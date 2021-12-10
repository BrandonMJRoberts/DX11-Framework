
// This is an output resource (RW = read/write)
RWTexture3D<float4> gOutput : register(u0);

// --------------------------------------------------------------------------------------------------------------------------- 

static const float PI                = 3.14159265f;
static const float earthRadius       = 6371000.0f;
static const float atmosphereRadius  = 80000.0f;

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
    
    float3 parameters = float3((textureCoords.x * textureCoords.x) * atmosphereRadius,            // X texture coord to the correct parameter for the equations
                               0.0f,                                                              // Leave at zero as we are going to calculate the two options below
                               tan((2 * textureCoords.y) - 1 + 0.26) * 0.75) / tan(1.26 * 0.75);  // Z texture coord to the parameter required
    
    // The two Y options, based off of the splitting in the texture
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

float3 IntersectionPointOnSphere(float3 startPoint, float3 direction, float radius, bool positiveValue)
{
    float pDotDir             = dot(startPoint, direction);
    float radiusSquared       = radius * radius;
    float magnitudePSquared   = pow(length(startPoint), 2);
    float magnitudeDirSquared = pow(length(direction), 2);
    
    float t         = 0.0f;
    float sqrtValue = (pDotDir * pDotDir) - (magnitudeDirSquared * (magnitudePSquared - radiusSquared));
    
    // Check to see if we have any valid values
    if(sqrtValue < 0.0f)
        return float3(0.0f, 0.0f, 0.0f);
    
    if(positiveValue)
    {
        t = -pDotDir + sqrt(sqrtValue);
        t /= magnitudeDirSquared;
    }
    else
    {
        t = -pDotDir - sqrt(sqrtValue);
        t /= magnitudeDirSquared;
    }
    
    float3 newPoint = startPoint + (t * direction);
    
    return newPoint;
}

// --------------------------------------------------------------------------------------------------------------------------- 

// Calculate the single scattering reaching point a from direction 'viewingDirection', with incident light direction 'lightDirection'
float SingleScattering(float3 pointOrigin, float3 viewingDirection, float3 lightDirection)
{
    // Calculate the intersection point of the light ray to the atmosphere, from the point passed in, using the negative viewing direction
    float3 pointB = IntersectionPointOnSphere(pointOrigin, viewingDirection, atmosphereRadius, true);
    
    // Calculate the step distance
    float stepSize = distance(pointOrigin, pointB) / INTEGRATION_STEPS;
    
    // Useful variables to not be re-defined
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
        currentPoint = pointOrigin + (stepSize * step * -viewingDirection);

        // Calculate the transmitance between the two points
        transmittance = Transmittance(pointOrigin, currentPoint);

        // Calculate the intersection point on the atmosphere from the current point, in the direction of the light back towards the sun
        pointC = IntersectionPointOnSphere(currentPoint, -lightDirection, atmosphereRadius, true);
        
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
    
    
    float3 rayleighScattering = totalInscatteringRayleigh * (RayleighCoefficient / (4 * PI));
    float mieRValue           = totalInscatteringMie      * (MieCoefficient.r    / (4 * PI));
    
    // Return back the data in the order required
    return float4(rayleighScattering.rgb, mieRValue);
}

// --------------------------------------------------------------------------------------------------------------------------- 

float4 multiplyQuat(float4 quat1, float4 quat2)
{
    return float4((quat1.r * quat2.r) - (quat1.g * quat2.g) - (quat1.b * quat2.b) - (quat1.a * quat2.a),
                  (quat1.r * quat2.g) + (quat1.g * quat2.r) + (quat1.b * quat2.a) - (quat1.a * quat2.b),
                  (quat1.r * quat2.b) - (quat1.b * quat2.a) + (quat1.b * quat2.r) + (quat1.a * quat2.g),
                  (quat1.r * quat2.a) + (quat1.g * quat2.b) - (quat1.b * quat2.g) + (quat1.a * quat2.r));
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
    // Divide by 128 as the texture is 128x128x128, therefore the thread counts go from 0 to 127, making the conversion to texture coords
    float3 textureCoord = float3(dispatchThreadID.x / 128.0f, 
                                 dispatchThreadID.y / 128.0f, 
                                 dispatchThreadID.z / 128.0f);
    
    // Convert the texture coords to the parameters required for the equations below
    float3 parameters = ConvertToParameters(textureCoord);
    
    // Now calculate the values used for the scattering equations based off of the values calculated above
    float3 up                         = float3(0.0f, 1.0f, 0.0);
    
    // The scattering calculation point is the planet's surface point + the height up we are
    // This is because we are making the assumption that all horizontal points give the same value
    // + float3(0.0f, 0.0f, 0.0f) because our planet's surface is the x-z plane
    float3 pointToCalculateScattering = (up * parameters.x) + float3(0.0f, 0.0f, 0.0f);
    
    // We need to rotate the up vector by the angle calculated at the start in order to get the viewDirection and the light direction
    
    // The rotation needs to be around the z axis in my case, but it should be around the 'forward' of the ground 
    // Do this using quaternions
    // parameters.y = cos(viewAngle)
    float4 viewingQuat          = float4(parameters.y / 2.0f, 0.0f, sin(acos(parameters.y) / 2.0f), 0.0f);
    float4 viewingQuatCongegate = float4(viewingQuat.x, -viewingQuat.y, -viewingQuat.z, -viewingQuat.w);
    float4 pointRotateQuat      = float4(0.0f, 0.0f, 1.0f, 0.0f); // The point (0, 1, 0) stored in a quaternon for rotating
    float4 rotatedPoint         = multiplyQuat(multiplyQuat(viewingQuat, pointRotateQuat), viewingQuatCongegate); // Apply the rotation : newPoint = Q * P * Q*
    
    // Extract the new direction from the quaterneon
    float3 viewingDirection     = rotatedPoint.yzw;
    
    // Now for the light direction
    float4 lightQuat            = float4(parameters.z / 2.0f, 0.0f, sin(acos(parameters.z) / 2.0f), 0.0f);
    float4 lightQuatCongegate   = float4(lightQuat.x, -lightQuat.y, -lightQuat.z, -lightQuat.w);
           pointRotateQuat      = float4(0.0f, 0.0f, 1.0f, 0.0f);
           rotatedPoint         = multiplyQuat(multiplyQuat(lightQuat, pointRotateQuat), lightQuatCongegate);
    
    // Extract the new direction from the quaterneon
    float3 lightDirection = rotatedPoint.yzw;
    
    // Now calculate the single scattering value for these parameters    
    float4 scattering = SingleScattering(pointToCalculateScattering, viewingDirection, lightDirection);
    
    // Write the final values out to the texture store
    gOutput[dispatchThreadID.xyz] = float4(scattering);
}

// --------------------------------------------------------------------------------------------------------------------------- 
