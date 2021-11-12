//--------------------------------------------------------------------------------------

cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
    float3 cameraPosition;
}

// The vertex shader outputs the position and texture coord
struct VS_OUTPUT
{
    float4 Pos          : SV_POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(float3 Pos : POSITION)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    // Calculate the final position
    output.Pos = mul(float4(Pos, 1.0f), World);
    output.Pos = mul(output.Pos,        View);
    output.Pos = mul(output.Pos,        Projection);

    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
    // Calculate the correct colour for the pixel based on atmospheric stuff

    // Do the texture lookup and return it back
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
