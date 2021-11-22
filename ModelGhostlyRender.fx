//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
    float3 cameraPosition;
}

cbuffer LightingData : register(b1)
{
    float4 lightColour;
    float3 lightDirection;
}

cbuffer MaterialData : register(b2)
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float  shinniness;
}

//--------------------------------------------------------------------------------------

struct VS_OUTPUT
{
    float4 Pos           : SV_POSITION;
    float4 WorldSpacePos : POSITION;
    float2 TexCoord      : TEXCOORD;
    float3 Normal        : NORMAL;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(float3 pos : POSITION, float2 textureCoord : TEXCOORD, float3 normal : NORMAL)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    // Calculate the final screen position of the vertex    
    output.Pos           = mul(float4(pos, 1.0f), World);
    output.WorldSpacePos = output.Pos;
    
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    
    // Pass the texture coord through
    output.TexCoord = textureCoord;
    
    // Convert the normal to world space
    output.Normal = mul(float4(normal, 0.0f), World);

    return output;
}

Texture2D    texurePallet : register(t0); // The texture being passed into the shader
SamplerState sampler1     : register(s0); // Set the sampler state

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    // First normalise the normal
    input.Normal = normalize(input.Normal);
    
    // Now extract the base colour from the texture
    float4 colourFromTexture = texurePallet.Sample(sampler1, input.TexCoord);
    
    // -----------------------------------------------------------------------------------
    // Lighting
    
    // Calculate the ambient component of the lighting
    float3 ambientPart = ambient * lightColour * colourFromTexture;
    
    // Now calculate the diffuse part
    float3 diffusePart = max(dot(lightDirection, input.Normal), 0.0f);
    diffusePart        = diffuse.rgb * (diffusePart * lightColour.rgb).rgb;
    
    // Now calculate the specular part
    // First calculate the reflection vector of the light by the normal
    float3 reflectVec = reflect(-lightDirection, input.Normal);
    
    // Now calculate the vector from the pixel to the camera
    float3 toEye = normalize(cameraPosition - input.WorldSpacePos.xyz);
    
    float3 specularPart = pow(max(dot(reflectVec, toEye), 0.0f), shinniness);
    
    // ------------------------------------------------------------------------------------
    
    // Pass out the final colour as a transparent, more white version of the normal colours
    float3 finalColour = ambientPart + specularPart + diffusePart;
    
    // Now make the final colour a whitey, ghostly colour
    finalColour = finalColour / length(finalColour);
    
    return float4(finalColour, 0.1f);
}
