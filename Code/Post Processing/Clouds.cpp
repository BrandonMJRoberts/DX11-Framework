#include "Clouds.h"
#include "../Maths/Commons.h"

#include <random>

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
	SetupNoiseTexture();

	SetupShaders();

	// Now generate the runtime data
	GenerateWeatherMapTexture();

	GenerateNoiseTexture();
}

void VolumetricClouds::BindNoiseTest()
{
	if (mShapeTexture)
		mShapeTexture->BindTextureToShaders(0, 1);
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

// This is the setup of the noise textures
void VolumetricClouds::SetupNoiseTexture()
{
	// Create the render buffer we are going to write the noise textures to
	mShapeTexture = new RenderBuffer(mShaderHandler, 512, 512, 1, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_USAGE_DEFAULT);

	// ------------------------------------------------------------

	// Create the shaders required for this render
	ID3D11VertexShader* shapeTextureVertex = nullptr;
	ID3D11PixelShader*  shapeTexturePixel  = nullptr;

	VertexShaderReturnData returnData = mShaderHandler.CompileVertexShader(L"DetailNoiseGenerator.fx", "VS");
	               shapeTextureVertex = returnData.vertexShader;

	shapeTexturePixel = mShaderHandler.CompilePixelShader(L"DetailNoiseGenerator.fx", "PS");

	// ------------------------------------------------------------

	// Now create the temporary vertex data for the render
	TextureVertex verticies[] =
	{
		{DirectX::XMFLOAT3(-1.0f,  1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f)}, // Top left
		{DirectX::XMFLOAT3( 1.0f,  1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f)}, // Top Right
		{DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f)}, // Bottom left
		{DirectX::XMFLOAT3( 1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f)}  // Bottom right
	};

	// Now setup the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	ID3D11InputLayout* inputLayout;

	if (!mShaderHandler.SetDeviceInputLayout(returnData.Blob, layout, 2, &inputLayout))
		return;

	// Bind the input layout
	mShaderHandler.SetInputLayout(inputLayout);

	// ------------------------------------------------------------

	// Now setup the buffers used for the render
	ID3D11Buffer* vertexBuffer;

	// Create the vertex buffer
	if (!mShaderHandler.CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, (D3D11_CPU_ACCESS_FLAG)0, verticies, sizeof(TextureVertex) * 6, &vertexBuffer))
		return;

	UINT stride = sizeof(TextureVertex);
	UINT offset = 0;

	// Bind the vertex buffer to the correct registers
	if (!mShaderHandler.BindVertexBuffersToRegisters(0, 1, &vertexBuffer, &stride, &offset))
		return;

	WORD indicies[] =
	{
		// Front
		0,1,2,
		1,3,2
	};

	// Create the index buffer
	ID3D11Buffer* indexBuffer;
	if (!mShaderHandler.CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, (D3D11_CPU_ACCESS_FLAG)0, indicies, sizeof(WORD) * 6, &indexBuffer))
		return;

	if (!mShaderHandler.BindIndexBuffersToRegisters(indexBuffer, DXGI_FORMAT_R16_UINT, 0))
		return;

	// ------------------------------------------------------------

	// Setup the constant buffer for the data we are going to pass in
	ID3D11Buffer* constantBufferLevel1 = nullptr;
	ID3D11Buffer* constantBufferLevel2 = nullptr;
	ID3D11Buffer* constantBufferLevel3 = nullptr;
	ID3D11Buffer* constantBufferLevel4 = nullptr;

	// ---------------------------------------------------------------------------------------------- //

	// Level 1

	// Calculate the data points for each layer of the noise
	std::vector<Vector2D> data;
	GenerateDataPointsForNoise(8, data);

	// Create the buffers we need, assigning the data to them
	if (!mShaderHandler.CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, (D3D11_CPU_ACCESS_FLAG)0, data.data(), sizeof(Vector2D) * data.size(), &constantBufferLevel1))
		return;

	// ---------------------------------------------------------------------------------------------- //

	// Level 2
	data.clear();
	GenerateDataPointsForNoise(16, data);

	if (!mShaderHandler.CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, (D3D11_CPU_ACCESS_FLAG)0, data.data(), sizeof(Vector2D) * data.size(), &constantBufferLevel2))
		return;

	// ---------------------------------------------------------------------------------------------- //

	// Level 3
	data.clear();
	GenerateDataPointsForNoise(32, data);

	if (!mShaderHandler.CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, (D3D11_CPU_ACCESS_FLAG)0, data.data(), sizeof(Vector2D) * data.size(), &constantBufferLevel3))
		return;

	// ---------------------------------------------------------------------------------------------- //

	// Level 4
	data.clear();
	GenerateDataPointsForNoise(64, data);

	if (!mShaderHandler.CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, (D3D11_CPU_ACCESS_FLAG)0, data.data(), sizeof(Vector2D) * data.size(), &constantBufferLevel4))
		return;

	// Now bind the buffers as we are going to use them shortly
	mShaderHandler.SetPixelShaderConstantBufferData(0, 1, &constantBufferLevel1);
	mShaderHandler.SetPixelShaderConstantBufferData(1, 1, &constantBufferLevel2);
	mShaderHandler.SetPixelShaderConstantBufferData(2, 1, &constantBufferLevel3);
	mShaderHandler.SetPixelShaderConstantBufferData(3, 1, &constantBufferLevel4);

	data.clear();

	// ---------------------------------------------------------------------------------------------- //
	 
	// Attach the texture as the render target
	mShapeTexture->BindRenderTargetAsActive();

	// Bind the shaders
	mShaderHandler.SetVertexShader(shapeTextureVertex);
	mShaderHandler.SetPixelShader(shapeTexturePixel);

	// Render the noise to the texture
	mShaderHandler.DrawIndexed(6, 0, 0);

	// Revert the render buffer change
	mShaderHandler.SetPriorRenderTarget();

	// Unbind the shaders
	mShaderHandler.SetVertexShader(nullptr);
	mShaderHandler.SetPixelShader(nullptr);

	// Clear up the memory allocated here as we dont need the shaders at any point later on
	shapeTextureVertex->Release();
	shapeTexturePixel->Release();

	constantBufferLevel1->Release();
	constantBufferLevel2->Release();
	constantBufferLevel3->Release();
	constantBufferLevel4->Release();

	vertexBuffer->Release();
	indexBuffer->Release();

	inputLayout->Release();
}

// --------------------------------------------------------------------------------- //

void VolumetricClouds::SetupShaders()
{

}

// --------------------------------------------------------------------------------- //

void VolumetricClouds::GenerateDataPointsForNoise(unsigned int slices, std::vector<Vector2D>& returnData)
{
	// We are only going to use 512x512 textures for this
	unsigned int pixelsPerGridBox = 512 / slices;

	Vector2D randomPoint;

	// Loop through the slices requested and calculate the random point we are going to add to the list
	for (unsigned int i = 0; i < slices; i++)
	{
		for (unsigned int j = 0; j < slices; j++)
		{
			randomPoint = Vector2D::zero;

			// First choose a random point within the range of the grid's pixels
			randomPoint.x = rand() % pixelsPerGridBox;
			randomPoint.y = rand() % pixelsPerGridBox;

			// Now add on the offset to get it into the correct box position
			randomPoint.x += (j * pixelsPerGridBox);
			randomPoint.y += (i * pixelsPerGridBox);

			// Now convert to texture coords
			returnData.push_back(Vector2D(randomPoint.x / 512.0f, randomPoint.y / 512.0f));
		}
	}
}

// --------------------------------------------------------------------------------- //