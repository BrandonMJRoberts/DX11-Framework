// ---------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------

cbuffer ConstantBuffer : register(b0)
{
    // Matricies for converting between spaces
    matrix World;
    matrix View;
    matrix Projection;
    
    float3 cameraPosition;
    float3 playerViewDirection;
}

cbuffer TimeOfDay : register(b1)
{
    // Used to calculate where the sun is, and therefore its direction
    // 1200 is straight up (midday), 0 = 2400 (midnight)
    float timeOfDay;
}

// The vertex shader outputs the position and texture coord
struct VS_OUTPUT
{
    float4 Pos          : SV_POSITION;
    float3 WorldPos     : WorldPosition; // Useful for the angle calculations
};

// ---------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(float3 Pos : POSITION)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    // Calculate the final position
    output.Pos      = mul(float4(Pos, 1.0f), World);
    
    output.WorldPos = output.Pos;
    
    output.Pos      = mul(output.Pos,        View);
    output.Pos      = mul(output.Pos,        Projection);

    return output;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------


// The scattering value LUT
Texture3D    preCalculatedLUT;
SamplerState samplerState;

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

// Angle is the angle between the direction of incident light, and the direction of the scattered light
float RayleighPhaseFunction(float angle)
{
    return 0.75 * (1.0f + (cos(angle) * cos(angle)));
}

// g is an asymmetry factor depicting the width of the forward lobe in Mie scattering (-1 to 1)
float MiePhaseFunction(float angle, float g)
{
    float firstPart  = (3.0f * (1.0f - (g * g)))          / (2.0f * (2.0f + (g * g)));
    float secondPart = (1.0f + (cos(angle) * cos(angle))) / pow(1.0f + (g * g) - (2.0f * g * (cos(angle) * cos(angle))), 1.5f);
    
    return firstPart * secondPart;
}

//--------------------------------------------------------------------------------------

float atmosphereHeight = 80000;
float earthRadius      = 6371000.0f;

float3 ConvertToTextureCoords(float height, float viewAngle, float sunAngle)
{
    // First asign the height parameter as it is the simplist to calculate
    float3 returnTextureCoords = float3(pow((height / atmosphereHeight), 0.5), 0.0f, 0.0f);
    
    // Now for the view angle 
    float cosView         = cos(viewAngle);
    float ch              = -(sqrt(height * ((2 * earthRadius) + height)) / (earthRadius + height));
    
    if(cosView > ch)
    {
        returnTextureCoords.y = (0.5f * pow(((cosView - ch) / (1 - ch)), 0.2f)) + 0.5f;
    }
    else
    {
        returnTextureCoords.y = 0.5f * pow((ch - cosView) / (ch + 1), 0.2f);
    }
    
    // Now for the sun angle
    float cosSun          = cos(sunAngle);
    returnTextureCoords.z = 0.5f * ((atan(max(cosSun, -0.1975f) * tan(1.26f * 1.1f)) / 1.1f) + (1.0f - 0.26f));
    
    // Return the values calculated
    return returnTextureCoords;
}

//--------------------------------------------------------------------------------------

static const float PI = 3.14159265f;

float GetSunAngleFromTime(float time)
{
    // Make sure that the time is in the correct range
    time %= 2400;
    
    // 1200 is stright up, so an angle of zero
    // 0,2400 are straight down, so an angle of PI
    
    // Convert to a 0->1 value for the range we want
    time /= 1200;
    
    bool loop = false;
    
    if(time > 1.0)
    {
        loop = true;
        time -= 1.0;
    }
    
    // Convert to the correct angle range
    time *= 2;
    time -= 1.0f;
    
    // Convert to the correct angle range
    time = acos(time);
    
    if(loop)
    {
        time = PI - time;
    }
    
    // Return the new value calculated
    return time;
}

//--------------------------------------------------------------------------------------

// Rayleigh coefficient for RGB values
// Calculated through the equation R(lambda) = 8pi^3 * ((n^2 - 1)^2) / (3 * Ne * lambda^4)
// Where n = refractive index of air ~1.0003
// and Ne is the molecular density of air at sea level ~ 2.545e25
// and lambda being (6.5e-7, 5.1e-7, 4.75e-7)
float3 RayleighCoefficient = float3(6.55e-6, 1.73e-5, 2.30e-5);

// The Mie coefficient, the paper says it is more complex to calculate than the Rayleigh coefficient
float3 MieCoefficient = float3(2e-6, 2e-6, 2e-6);

float4 PS(VS_OUTPUT input) : SV_Target
{
    // ------------------------------------------------------------------------------------
    
    // Calculate the correct colour for the pixel based on atmospheric stuff
    // Calculate the height, view angle and the sun angle based off the current data
    float height = cameraPosition.y;
    
    // The angle between the view direction of the player and 'up'
    float3 up       = float3(0.0f, 1.0f, 0.0f);
    
    // No divide by the magnitides as 'up' is already normalised and view direction should be normalised before being passed into the shader
    float viewAngle = acos(dot(up, playerViewDirection));
    
    // The angle between the vector to the sun and 'up'
    float sunAngle = GetSunAngleFromTime(timeOfDay);
    
    // ------------------------------------------------------------------------------------
    
    // Fetch the scattered light values
    float3 textureCoords  = ConvertToTextureCoords(height, viewAngle, sunAngle);
    
    // ------------------------------------------------------------------------------------
    
    // The scattered light stored is Rayleigh scattering in RGB, and a converted MieRGB into a single value using the other three values
    float4 scatteredLight = preCalculatedLUT.Sample(samplerState, textureCoords);
    
    float3 rayleighLight = scatteredLight.rgb;
    float3 mieLight      = rayleighLight * (scatteredLight.a / rayleighLight.r) * (RayleighCoefficient.r / MieCoefficient.r) * (MieCoefficient / RayleighCoefficient);
    
    // ------------------------------------------------------------------------------------
    
    // Calculate the intensity of light based off of the current sun angle
    float intensity = cos(sunAngle);
    
    // Multiply by the intensity of incident light
    rayleighLight *= intensity;
    mieLight      *= intensity;
    
    // ------------------------------------------------------------------------------------
    
    // Angle is the angle between the direction of incident light, and the direction of the scattered light
    float angle;
    
    if(viewAngle > sunAngle)
        angle = viewAngle - sunAngle;
    else
        angle = sunAngle - viewAngle;
    
    // Apply the respective phase functions
    rayleighLight *= RayleighPhaseFunction(angle);
    mieLight      *= MiePhaseFunction(angle, 0.0f);
    
    // ------------------------------------------------------------------------------------
    
    // Combine into a single RGB value
    float3 finalRGB = rayleighLight + mieLight;
    
    // ------------------------------------------------------------------------------------
    
    // Return the final colour for rendering
    //return float4(finalRGB, 1.0f);
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------