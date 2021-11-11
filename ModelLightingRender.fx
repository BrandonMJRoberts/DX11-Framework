//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
}

//--------------------------------------------------------------------------------------

struct VS_OUTPUT
{
    float4 Pos          : SV_POSITION;
    float2 TexCoord     : TEXCOORD;
    float3 Normal       : NORMAL;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(float3 pos : POSITION, float2 textureCoord : TEXCOORD, float3 normal : NORMAL)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    // Calculate the final screen position of the vertex    
    output.Pos = mul(float4(pos, 1.0f), World);
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    
    // Pass the texture coord through
    output.TexCoord = textureCoord;
    
    // Pass the normal through for lighting calculations
    output.Normal = normal;

    return output;
}

Texture2D    texurePallet : register(t0); // The texture being passed into the shader
SamplerState sampler1     : register(s0); // Set the sampler state

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    // For now just use the texture colour
    return texurePallet.Sample(sampler1, input.TexCoord);
}
