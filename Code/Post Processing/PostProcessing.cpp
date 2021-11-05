#include "PostProcessing.h"
#include "../Maths/Commons.h"
#include "../Maths/Constants.h"
#include "../GameScreens/ScreenManager.h"

// ------------------------------------------------------------------ 

PostProcessing::PostProcessing(ShaderHandler& shaderHandler)
	: mDepthStencilViewPostProcessing(nullptr),
	  mDepthTextureResourceView(nullptr),
	  mClouds(nullptr),
	  mPixelShader(nullptr),
	  mVertexShader(nullptr),
	  mShaderHandler(shaderHandler),
	  mShaderInputLayout(nullptr),
	  mIndexBuffer(nullptr),
	  mPostProcessingRenderBuffer(nullptr),
	  mSamplerState(nullptr)
{
	LoadInShaders();

	SetupBuffers();

	SetupRenderTextures();

	// Create the clouds
	mClouds = new VolumetricClouds();
}

// ------------------------------------------------------------------ 

PostProcessing::~PostProcessing()
{
	delete mClouds;
	mClouds = nullptr;

	delete mPostProcessingRenderBuffer;
	mPostProcessingRenderBuffer = nullptr;

	if (mDepthStencilViewPostProcessing)
	{
		mDepthStencilViewPostProcessing->Release();
		mDepthStencilViewPostProcessing = nullptr;
	}

	if (mDepthTextureResourceView)
	{
		mDepthTextureResourceView->Release();
		mDepthTextureResourceView = nullptr;
	}

	if (mPixelShader)
	{
		mPixelShader->Release();
		mPixelShader = nullptr;
	}

	if (mVertexShader)
	{
		mVertexShader->Release();
		mVertexShader = nullptr;
	}

	if (mShaderInputLayout)
	{
		mShaderInputLayout->Release();
		mShaderInputLayout = nullptr;
	}

	delete mSamplerState;
	mSamplerState = nullptr;

	if (mIndexBuffer)
	{
		mIndexBuffer->Release();
		mIndexBuffer = nullptr;
	}
}

// ------------------------------------------------------------------ 

void PostProcessing::RenderFinal()
{
	// Re-bind the default render target so we can render to it
	mShaderHandler.SetDefaultRenderTarget();

	// Set the correct viewport
	mShaderHandler.SetViewport((float)GameScreenManager::ScreenWidth, (float)GameScreenManager::ScreenHeight, 0.0f, 1.0f, 0, 0);

	mShaderHandler.SetInputLayout(mShaderInputLayout);

	// Set the shaders we are going to use
	mShaderHandler.SetVertexShader(mVertexShader);
	mShaderHandler.SetPixelShader(mPixelShader);

	if(mPostProcessingRenderBuffer)
		mPostProcessingRenderBuffer->BindTextureToShaders(0, 1);

	if(mSamplerState)
		mSamplerState->BindSamplerState(0, 1);

	UINT stride = sizeof(TextureVertex);
	UINT offset = 0;

	// Bind the vertex buffer to the correct registers
	mShaderHandler.BindVertexBuffersToRegisters(0, 1, &mVertexBuffer, &stride, &offset);
	mShaderHandler.BindIndexBuffersToRegisters(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Now draw the final screen
	mShaderHandler.DrawIndexed(6, 0, 0);

	// Unbind the shaders
	mShaderHandler.SetVertexShader(NULL);
	mShaderHandler.SetPixelShader(NULL);
}

// ------------------------------------------------------------------ 

void PostProcessing::RenderVolumetricClouds()
{
	// Now render the clouds now that everything has been setup properly
	if (mClouds)
		mClouds->Render();
}

// ------------------------------------------------------------------ 

void PostProcessing::Render()
{
	// Render the post processing stuff - starting with clouds
	RenderVolumetricClouds();



	// Render the post processed scene to the screen
	RenderFinal();
}

// ------------------------------------------------------------------ 

void PostProcessing::BindRenderTarget()
{
	if (mPostProcessingRenderBuffer && !mDepthStencilViewPostProcessing)
		return;

	// Bind the render target and depth buffer
	mPostProcessingRenderBuffer->BindRenderTargetAsActive(mDepthStencilViewPostProcessing);
}

// ------------------------------------------------------------------ 

void PostProcessing::ClearRenderTarget()
{
	if (mPostProcessingRenderBuffer && !mDepthStencilViewPostProcessing)
		return;

	mPostProcessingRenderBuffer->ClearRenderBuffer(mDepthStencilViewPostProcessing); 
}

// ------------------------------------------------------------------ 

void PostProcessing::LoadInShaders()
{
	// Load in the vertex and pixel shader
	VertexShaderReturnData returnData    = mShaderHandler.CompileVertexShader(L"PostProcessingFinalRender.fx", "VS");
	                       mVertexShader = returnData.vertexShader;

    mPixelShader = mShaderHandler.CompilePixelShader(L"PostProcessingFinalRender.fx", "PS");

	// Define how the data should be passed into the shader
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if (!mShaderHandler.SetDeviceInputLayout(returnData.Blob, layout, 2, &mShaderInputLayout))
		return;

	mShaderHandler.SetVertexShader(mVertexShader);
	mShaderHandler.SetPixelShader(mPixelShader);
}

// ------------------------------------------------------------------ 

void PostProcessing::SetupRenderTextures()
{
	mPostProcessingRenderBuffer = new RenderBuffer(mShaderHandler, GameScreenManager::ScreenWidth, GameScreenManager::ScreenHeight, 1, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_USAGE_DEFAULT);

	// ------------------------------------------------------------------------------------------------- //

	// Depth texture setup

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width              = GameScreenManager::ScreenWidth;
	depthStencilDesc.Height				= GameScreenManager::ScreenHeight;
    depthStencilDesc.MipLevels          = 1;
    depthStencilDesc.ArraySize          = 1;
    depthStencilDesc.Format             = DXGI_FORMAT_R24G8_TYPELESS;
    depthStencilDesc.SampleDesc.Count   = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage              = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    depthStencilDesc.CPUAccessFlags     = 0;
    depthStencilDesc.MiscFlags          = 0;

	// Now create the depth / stencil view
	ID3D11Texture2D* tempDepthTexture = nullptr;

	// Create the texture
	if (!mShaderHandler.CreateTexture2D(&depthStencilDesc, nullptr, &tempDepthTexture))
		return;
	
	D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc;
	depthDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthDesc.Flags              = 0;
	depthDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view
	if (!mShaderHandler.CreateDepthStencilView(tempDepthTexture, &depthDesc, &mDepthStencilViewPostProcessing))
		return;

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format                    = DXGI_FORMAT_R32G32B32A32_FLOAT;
	shaderResourceViewDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels       = 1;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc = shaderResourceViewDesc;
	resourceDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

	// Now create the shader resource from it - so that we can pass it into shaders later on
	mShaderHandler.CreateShaderResourceView(tempDepthTexture, &resourceDesc, &mDepthTextureResourceView);

	// Release the temp texture's memory 
	tempDepthTexture->Release();
	tempDepthTexture = nullptr;

	// Create the sampler state
	mSamplerState = new SamplerState(mShaderHandler, D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, 0.0f, 0, 1, 0.0f, 0.0f, 0.0f, 0.0f, D3D11_COMPARISON_ALWAYS);
}

// ------------------------------------------------------------------ 

void PostProcessing::SetupBuffers()
{
	// Define the verticies we need for this to work
	TextureVertex verticies[] =
	{
		{DirectX::XMFLOAT3(-1.0f,  1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f)}, // Top left
		{DirectX::XMFLOAT3( 1.0f,  1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f)}, // Top Right
		{DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f)}, // Bottom left
		{DirectX::XMFLOAT3( 1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f)}  // Bottom right
	};

	// Create the vertex buffer
	if (!mShaderHandler.CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, (D3D11_CPU_ACCESS_FLAG)0, verticies, sizeof(TextureVertex) * 4, &mVertexBuffer))
		return;

	UINT stride = sizeof(TextureVertex);
	UINT offset = 0;

	// Bind the vertex buffer to the correct registers
	if (!mShaderHandler.BindVertexBuffersToRegisters(0, 1, &mVertexBuffer, &stride, &offset))
		return;

	WORD indicies[] =
	{
		// Front
		0,1,2,
		1,3,2
	};

	// Create the index buffer
	if (!mShaderHandler.CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, (D3D11_CPU_ACCESS_FLAG)0, indicies, sizeof(WORD) * 6, &mIndexBuffer))
		return;

	if (!mShaderHandler.BindIndexBuffersToRegisters(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0))
		return;

	if (!mShaderHandler.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST))
		return;
}

// ------------------------------------------------------------------ 