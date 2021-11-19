
cbuffer levelOneDetailPoints
{
    // 8 slices on each axis, stored packed two points to one vector4
    float4 detailPointsLvl1[32];
};

cbuffer levelTwoDetailPoints
{
    // 16 slices on each axis, stored packed two points to one vector4
    float4 detailPointsLvl2[128];
};

cbuffer levelThreeDetailPoints
{
    // 32 slices on each axis, stored packed two points to one vector4
    float4 detailPointsLvl3[512];
};

cbuffer levelFourDetailPoints
{
    // 64 slices on each axis, stored packed two points to one vector4
    float4 detailPointsLvl4[2048];
};

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos          : SV_POSITION;
    float2 textureCoord : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float3 Pos : POSITION, float2 textureCoord : TEXCOORD0)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    // Pass the position and texture coord through
    output.Pos          = float4(Pos, 1.0f);
    output.textureCoord = textureCoord;

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float ShortestDistanceFromSurroundingPoints(int level, int centralGridBoxIndex, float2 textureCoord)
{
    int    indexBeingChecked;
    float  shortestDistance = 300000000000000000000000.0f;
    float  distanceBeingChecked;
    float2 pointBeingChecked;
    
    switch (level)
    {
        case 1:
            int row    = 0;
            int column = 0;
        
            // Start with the row above the point
            for (int i = 0; i < 9; i++)
            {
                column            = (i % 3) - 1;
                row               = (i / 3) - 1;
                indexBeingChecked = (centralGridBoxIndex + row) + (8 * column);
            
                if (indexBeingChecked < 0)
                    indexBeingChecked = 64 - abs(indexBeingChecked);
                else if (indexBeingChecked > 64)
                    indexBeingChecked = indexBeingChecked % 64;
            
                // Now calculate the distance
                if (indexBeingChecked % 2 == 0)
                    pointBeingChecked    = detailPointsLvl1[indexBeingChecked / 2];
                else
                    pointBeingChecked = detailPointsLvl1[(indexBeingChecked / 2) + 1];
            
                distanceBeingChecked = ((pointBeingChecked.x - textureCoord.x) * (pointBeingChecked.x - textureCoord.x)) + ((pointBeingChecked.y - textureCoord.y) * (pointBeingChecked.y - textureCoord.y));
            
                if (distanceBeingChecked < shortestDistance)
                    shortestDistance = distanceBeingChecked;
            }
        
            // Return the actual distance of the shortest distance
            return sqrt(shortestDistance);
        
        break;
        
        default:
        break;
    }
    
    return 1.0f;
}

//--------------------------------------------------------------------------------------

float WorleyNoiseCalculation(int level, float2 currentTexCoord)
{
    // First calculate which grid cell the current pixel falls within using the texture coords passed in
    float2 pixelCoord          = float2(currentTexCoord.x * 512, currentTexCoord.y * 512);
    uint   pixelsPerGridPosition;
    float2 gridBox             = float2(0, 0);
    uint   indexOfPointsStored = 0;
    float  shortestDistance    = 0.0f;
    
    switch (level)
    {
        case 1:
            // Find the amount of pixels for each grid we are traversing
            pixelsPerGridPosition = 512 / 8;
        
            // Calculate what grid box the pixel is in
            gridBox = float2((uint)pixelCoord.x / pixelsPerGridPosition,
                             (uint)pixelCoord.y / pixelsPerGridPosition);
        
            // Find the index of the grid that this pixel is in
            indexOfPointsStored = gridBox.x * (gridBox.y * 8);
        
            // Now calculate the distance to the closest point based off of the grid position, returning back the shortest distance
            return ShortestDistanceFromSurroundingPoints(1, indexOfPointsStored, currentTexCoord);
        
        case 2:
            pixelsPerGridPosition = 512 / 16;

            // Calculate what grid box the pixel is in
            gridBox               = float2(pixelCoord.x / pixelsPerGridPosition, 
                                           pixelCoord.y / pixelsPerGridPosition);
        
            // Find the index of the grid that this pixel is in
            indexOfPointsStored   = gridBox.x * gridBox.y;
        
            // Now calculate the distance to the closest point based off of the grid position, returning back the shortest distance
            return ShortestDistanceFromSurroundingPoints(2, indexOfPointsStored, currentTexCoord);
        
        case 3:
            pixelsPerGridPosition = 512 / 32;

             // Calculate what grid box the pixel is in
            gridBox               = float2(pixelCoord.x / pixelsPerGridPosition, 
                                           pixelCoord.y / pixelsPerGridPosition);
        
            // Find the index of the grid that this pixel is in
            indexOfPointsStored   = gridBox.x * gridBox.y;
        
            // Now calculate the distance to the closest point based off of the grid position, returning back the shortest distance
            return ShortestDistanceFromSurroundingPoints(3, indexOfPointsStored, currentTexCoord);
        
        
        case 4:
            pixelsPerGridPosition = 512 / 64;

             // Calculate what grid box the pixel is in
            gridBox               = float2(pixelCoord.x / pixelsPerGridPosition, 
                                           pixelCoord.y / pixelsPerGridPosition);
        
            // Find the index of the grid that this pixel is in
            indexOfPointsStored   = gridBox.x * gridBox.y;
        
            // Now calculate the distance to the closest point based off of the grid position, returning back the shortest distance
            return ShortestDistanceFromSurroundingPoints(4, indexOfPointsStored, currentTexCoord);
        
        default:
            return 1.0f;
    }
}

//--------------------------------------------------------------------------------------

float4 PS(VS_OUTPUT input) : SV_Target
{
    // Calculate the noise values
    float levelOneNoise   = WorleyNoiseCalculation(1, input.textureCoord);
    //float levelTwoNoise   = WorleyNoiseCalculation(2, input.textureCoord);
    //float levelThreeNoise = WorleyNoiseCalculation(3, input.textureCoord);
    //float levelFourNoise  = WorleyNoiseCalculation(4, input.textureCoord);

    return float4(levelOneNoise, levelOneNoise, levelOneNoise, levelOneNoise);
}

//--------------------------------------------------------------------------------------