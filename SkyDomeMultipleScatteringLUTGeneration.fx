
// This is an output resource (RW = read/write)
RWTexture3D<float4> gOutput : register(u0);

// --------------------------------------------------------------------------------------------------------------------------- 

static const float PI = 3.14159265f;

float3 ConvertToParameters(float3 textureCoords)
{
    // Convert from the texture coord to the correct parameter range as defined in the paper
    // X = x^2 * H(atm) (H(atmosphere) = 8000)
    // Z = tan((2y - 1 + 0.26)*0.75) / tan(1.26 * 0.75)
    
    // For y > 0.5
    // Y = ch + ((y - 0.5)^5) * (1 - ch)
    
    // For y <= 0.5
    // Y = ch - (y^5) * (1 + ch)
    
    // Where ch = -(sqrt(h * (2(planetRadius + h))) / (planetRadius + h))
    // Radius of earth = 6,371km = 6,371,000m
    
    float3 parameters = float3((textureCoords.x * textureCoords.x) * 80000,                       // X texture coord to the correct parameter for the equations
                               0.0f,                                                              // Leave at zero as we are going to calculate the two options below
                               tan((2 * textureCoords.y) - 1 + 0.26) * 0.75) / tan(1.26 * 0.75);  // Z texture coord to the parameter required
    
    // The two Y options, based off of the splitting in the texture
    float earthRadius = 6371000.0f;
    float ch          = -(sqrt(parameters.x * ((2 * earthRadius) + parameters.x)) / (earthRadius + parameters.x));
    
    if (textureCoords.y > 0.5f)
    {
        parameters.y = ch + (pow((textureCoords.y - 0.5f), 5) * (1 - ch));
    }
    else
    {
        parameters.y = ch - ((pow(textureCoords.y, 5)) * (1 + ch));
    }
    
    // Return back the new values
    return parameters;
}

// --------------------------------------------------------------------------------------------------------------------------- 

// Rayleigh coefficient for RGB values
// Calculated through the equation R(lambda) = 8pi^3 * ((n^2 - 1)^2) / (3 * Ne * lambda^4)
// Where n = refractive index of air ~1.0003
// and Ne is the molecular density of air at sea level ~ 2.545e25
// and lambda being (6.5e-7, 5.1e-7, 4.75e-7)
float3 RayleighCoefficient = float3(6.55e-6, 1.73e-5, 2.30e-5);

// The Mie coefficient, the paper says it is more complex to calculate than the Rayleigh coefficient
float3 MieCoefficient = float3(2e-6, 2e-6, 2e-6);

// --------------------------------------------------------------------------------------------------------------------------- 

// The density function scales the scattering coefficients to an arbitrary height, by representing the change in molecular and aerosol density
// We make the assumption that the molecular density decreases with an exponential rate with respect to the height above earth's surface 
float DensityRayleigh(float height)
{
    // 8000.0f is the height above sea level for this type of scattering to occur
    return exp(-(height / 8000.0f));
}

float DensityMie(float height)
{
    // 1200.0f is the height above sea level for this type of scattering to occur
    return exp(-(height / 1200.0f));
}

// --------------------------------------------------------------------------------------------------------------------------- 

// Define the amount of steps used for the integration solution
float INTEGRATION_STEPS = 50.0f;

// Transmittance describes the attenuation of light between two points
// This transmittance function takes ozone into account to get more realistic results
// The important part of this function is that the transmittance equation has an integral in it, which means we need to solve it using a method
// The method used in this function is the trapezoidal numerical integration approach
float Transmittance(float3 pointA, float3 pointB)
{
    float stepSize          = distance(pointB, pointA) / INTEGRATION_STEPS;
    
    // Find the direction of the light ray
    float3 direction        = normalize(pointB - pointA);

    float3 currentPoint           = pointA;
    
    float  currentDensityRayleigh = 0.0f;
    float  totalDensityRayleigh   = 0.0f;
    
    float  currentDensityMie      = 0.0f;
    float  totalDensityMie        = 0.0f;
    
    float previousDensityMie      = 0.0f;
    float previousDensityRayleigh = 0.0f;
    
    for (uint step = 0; step < INTEGRATION_STEPS; step++)
    {
        // Calculate the current point
        currentPoint = pointA + (stepSize * step * direction);
        
        // Get the current density at this point, at the height off of the ground that this point is
        // As our system has a completly flat ground, we can just use the y component
        currentDensityMie      = DensityMie(currentPoint.y);
        currentDensityRayleigh = DensityRayleigh(currentPoint.y);
        
        // Calculate the new total density
        totalDensityMie      += (currentDensityMie + previousDensityMie) / (2.0f * stepSize);
        totalDensityRayleigh += (currentDensityRayleigh + previousDensityRayleigh) / (2.0f * stepSize);
        
        // Now swap the prior and current density values
        previousDensityRayleigh = currentDensityRayleigh;
        previousDensityMie      = currentDensityMie;
    }

    // Apply the coefficients
    float transmitance = exp(-((totalDensityMie * RayleighCoefficient) + (totalDensityMie * MieCoefficient)));

    return transmitance;
}

// --------------------------------------------------------------------------------------------------------------------------- 

// Calculate the single scattering reaching point a from direction 'direction', with incident light direction 'lightDirection'
float SingleScattering(float3 pointA, float3 direction, float3 lightDirection)
{
    // Calculate the intersection point of the light ray to the atmosphere, from the point passed in, using the negative direction
    float3 pointB = 
    
    // Calculate the step distance
    float stepSize = distance(pointA, pointB) / INTEGRATION_STEPS;
    
    float3 currentPoint;
    float3 pointC;
    
    float  transmittance;
    float  currentInscatteringMie       = 0.0f;
    float  currentInscatteringRayleigh  = 0.0f;
    
    float totalInscatteringMie          = 0.0f;
    float totalInscatteringRayleigh     = 0.0f;
    
    float previousInscatteringMie       = 0.0f;
    float previousInscatteringRayleigh  = 0.0f;
    
    for (uint step = 0; step < INTEGRATION_STEPS; step++)
    {
        // Calculate the current point on the ray back to the atmosphere intersection point
        currentPoint = pointA + (stepSize * step * -direction);

        // Calculate the transmitance between the two points
        transmittance = Transmittance(pointA, currentPoint);

        // Calculate the intersection point on the atmosphere from the current point, in the direction of the light back towards the sun
        pointC =
        
        // Calculate the transmittance from the current point, to the atmosphere along the sun-light path
        transmittance += Transmittance(currentPoint, pointC);
        
        // Calculate the current inscattering, of the Mie components
        currentInscatteringMie = DensityMie(currentPoint.y) * transmittance;

        // Calculate the current inscattering, of the Rayleigh components 
        currentInscatteringRayleigh = DensityRayleigh(currentPoint.y) * transmittance;

        // Apply this scattering to the running total, Mie
        totalInscatteringMie += (currentInscatteringMie + previousInscatteringMie) / (2 * stepSize);
        
        // Apply this scattering to the running total, Rayleigh
        totalInscatteringRayleigh += (currentInscatteringRayleigh + previousInscatteringRayleigh) / (2 * stepSize);
        
        // Now pass accross the previous and current values
        previousInscatteringMie      = currentInscatteringMie;
        previousInscatteringRayleigh = currentInscatteringRayleigh;
    }

    totalInscatteringMie      *= MieCoefficient      / (4.0f * PI) * 
    totalInscatteringRayleigh *= RayleighCoefficient / *

}

// Gathered light function required for the multiple scattering calculations
float gatheredLight(float3 pointToGet, float3 direction, float3 lightDirection, float thetaSun)
{
    float gathered   = 0.0f;
    for (float theta = 0; theta < 2 * PI; theta += ((2 * PI) / INTEGRATION_STEPS))
    {
        gathered += fetchScattering(pointToGet.y, theta, thetaSun);
    }
    
    gathered *= (4 * PI) / INTEGRATION_STEPS;
    
    return gathered;
}

// Calculate the multiple scattering reaching point 'pointA' from the direction vector 'direction', with incident light direction 'lightDirection'
float MultipleScattering(float pointA, float3 direction, float3 lightDirection)
{
    // Calculate the intersection point of the ray from pointA, in the negative direction vector, to the atmosphere 
    float3 pointB =
    
    // Calculate the step size
    float stepSize = distance(pointA, pointB) / INTEGRATION_STEPS;
    
    // Now loop through the steps
    float3 currentPoint;
    float  transmittance;
    
    float currentInscatteringMie;
    float currentInscatteringRayleigh;
    
    float previousInscatteringMie      = 0.0f;
    float previousInscatteringRayleigh = 0.0f;
    
    float totalInscatteringMie      = 0.0f;
    float totalInscatteringRayleigh = 0.0f;
    
    for (uint step = 0; step < INTEGRATION_STEPS; step++)
    {
        // Calculate the current point
        currentPoint = pointA + (stepSize * step * -direction);

        // Get the transmittance for this point
        transmittance = Transmittance(pointA, currentPoint);

        // Calculate the current scattering
        currentInscatteringMie      = gatheredLight(currentPoint, direction, lightDirection) * DensityMie(currentPoint.y)      * transmittance;
        currentInscatteringRayleigh = gatheredLight(currentPoint, direction, lightDirection) * DensityRayleigh(currentPoint.y) * transmittance;
        
        // Apply the scattering calculated to the total
        totalInscatteringMie      += (currentInscatteringMie + previousInscatteringMie) / (2 * stepSize);
        totalInscatteringRayleigh += (currentInscatteringRayleigh + previousInscatteringRayleigh) / (2 * stepSize);
        
        previousInscatteringRayleigh = currentInscatteringRayleigh;
        //previousInscatteringMie      = currentInscatteringMie; // Not included in the paper for some reason
    }

}

// --------------------------------------------------------------------------------------------------------------------------- 

// The amount of threads in the thread group
[numthreads(8, 8, 8)]
void main(int3 dispatchThreadID : SV_DispatchThreadID)
{
    // This shader pre-computes the scattering equations and stores them in a 3D texture
    // The x-coord represents the height
    // The y-coord represents the view angle
    // The z-coord represents the sun angle
    
    // Firstly we need to convert from the thread ID passed in, to a texture coord
    // Divide by 128 as the texture is 128x128x128, therefore the thread counts go from 1 to 128, making the conversion to texture coords
    float3 textureCoord = float3(dispatchThreadID.x / 128.0f, 
                                 dispatchThreadID.y / 128.0f, 
                                 dispatchThreadID.z / 128.0f);
    
    // Convert the texture coords to the parameters required for the equations below
    float3 parameters = ConvertToParameters(textureCoord);
    
    // Now calculate the final scattering values for these parameters
    float3 rayleighLightScattering;
    float3 mieLightScattering;
    
    // Write the final values out to the texture store
    gOutput[dispatchThreadID.xyz] = float4(rayleighLightScattering.rgb, mieLightScattering.r);
}

// --------------------------------------------------------------------------------------------------------------------------- 
