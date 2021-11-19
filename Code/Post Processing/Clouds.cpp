#include "Clouds.h"
#include "../Maths/Commons.h"

// --------------------------------------------------------------------------------- //

VolumetricClouds::VolumetricClouds(ShaderHandler& shaderHandler)
	: mGlobalCoverage()
	, mGlobalDensity()
	, mShaderHandler(shaderHandler)
	, mWeatherMap(nullptr)
	, mShapeTexture(nullptr)
	, mBlueNoiseTexture(nullptr)
{
	// Initial setup of internal data
	SetupTextures();

	SetupShaders();

	// Now generate the runtime data
	GenerateWeatherMapTexture();

	GenerateNoiseTexture();
}

// --------------------------------------------------------------------------------- //

VolumetricClouds::~VolumetricClouds()
{

}

// --------------------------------------------------------------------------------- //

void VolumetricClouds::Render()
{
	// First generate the colour/alpha texture of the clouds


	// Now combine it with the depth buffer of the normal scene to get the final render
}

// --------------------------------------------------------------------------------- //

void VolumetricClouds::Update(const float deltaTime)
{
	// Update the texture coords so the clouds can move
	mTextureCoordOffset += mMovementSpeed * deltaTime;

	mTextureCoordOffset.LoopAroundOne();
}

// --------------------------------------------------------------------------------- //

void VolumetricClouds::GenerateWeatherMapTexture()
{

}

// --------------------------------------------------------------------------------- //

void VolumetricClouds::GenerateNoiseTexture()
{

}

// --------------------------------------------------------------------------------- //

void VolumetricClouds::SetupTextures()
{
	// Generate the shape texture data
	mShapeTexture = new Texture2D(mShaderHandler, 512, 512, 1, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT);

	// Create the shaders required for this render
	ID3D11VertexShader* shapeTextureVertex = nullptr;
	ID3D11PixelShader*  shapeTexturePixel  = nullptr;

	VertexShaderReturnData returnData = mShaderHandler.CompileVertexShader(L"", "VS");
	               shapeTextureVertex = returnData.vertexShader;

	shapeTexturePixel = mShaderHandler.CompilePixelShader(L"", "PS");

	// Now create the temporary vertex data for the render
	TextureVertex verticies[] =
	{
		{DirectX::XMFLOAT3(-1.0f,  1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f)}, // Top left
		{DirectX::XMFLOAT3( 1.0f,  1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f)}, // Top Right
		{DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f)}, // Bottom left
		{DirectX::XMFLOAT3( 1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f)}  // Bottom right
	};

	// Bind the texture
	mShapeTexture->BindTextureToShaders(0, 1);

	// Bind the shaders
	mShaderHandler.SetPixelShader(shapeTexturePixel);

	// Render the noise to the texture
	mShaderHandler.Draw(6, 0);

	// Revert the render buffer change
	mShaderHandler.SetPriorRenderTarget();
}

// --------------------------------------------------------------------------------- //

void VolumetricClouds::SetupShaders()
{

}

// --------------------------------------------------------------------------------- //