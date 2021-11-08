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
    float4 Normal       : NORMAL;
    float2 TextureCoord : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION, float4 Normal : NORMAL, float2 TextureCoord : TEXCOORD)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    // Calculate the final screen position of the vertex
    output.Pos       = mul( Pos, World );
    output.Pos       = mul( output.Pos, View );
    output.Pos       = mul( output.Pos, Projection );
    
    // Pass the texture coord through
    output.TextureCoord = TextureCoord;
    
    // Pass the normal through for lighting calculations
    output.Normal = Normal;

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
    return texurePallet.Sample(sampler1, input.TextureCoord);
}
