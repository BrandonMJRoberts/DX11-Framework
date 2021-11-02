//--------------------------------------------------------------------------------------

// The vertex shader outputs the position and texture coord
struct VS_OUTPUT
{
    float4 Pos          : SV_POSITION;
    float2 textureCoord : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION, float2 TextureCoord : TEXCOORD)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    // Pass the data through
    output.Pos          = Pos;
    output.textureCoord = TextureCoord;

    return output;
}


Texture2D     finalScreenRenderImage      : register(t0); // The texture being passed into the shader
SamplerState sampler1                     : register(s0);// Set the sampler state

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    // Do the texture lookup and return it back
    return finalScreenRenderImage.Sample(sampler1, input.textureCoord);
}
