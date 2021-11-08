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
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    // Calculate the final screen position of the vertex
    output.Pos       = mul( Pos, World );
    output.Pos       = mul( output.Pos, View );
    output.Pos       = mul( output.Pos, Projection );

    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    // For now just use the texture colour
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
