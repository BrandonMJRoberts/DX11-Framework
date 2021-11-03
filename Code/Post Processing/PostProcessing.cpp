#include "PostProcessing.h"
#include "../Maths/Commons.h"
#include "../Maths/Constants.h"
#include "../GameScreens/ScreenManager.h"

// ------------------------------------------------------------------ 

PostProcessing::PostProcessing(ShaderHandler& shaderHandler)
	: mPostProcessingTexture(nullptr),
	  mRenderTargetViewPostProcessing(nullptr),
	  mShaderResourceViewPostProcessing(nullptr),
	  mDepthStencilViewPostProcessing(nullptr),
	  mDepthTextureResourceView(nullptr),
	  mClouds(nullptr),
	  mPixelShader(nullptr),
	  mVertexShader(nullptr),
	  mShaderHandler(shaderHandler),
	  mShaderInputLayout(nullptr),
	  mTextureSamplerState(nullptr),
	  mIndexBuffer(nullptr)
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

	if (mPostProcessingTexture)
	{
		mPostProcessingTexture->Release();
		mPostProcessingTexture = nullptr;
	}

	if (mRenderTargetViewPostProcessing)
	{
		mRenderTargetViewPostProcessing->Release();
		mRenderTargetViewPostProcessing = nullptr;
	}

	if (mShaderResourceViewPostProcessing)
	{
		mShaderResourceViewPostProcessing->Release();
		mShaderResourceViewPostProcessing = nullptr;
	}

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

	if (mTextureSamplerState)
	{
		mTextureSamplerState->Release();
		mTextureSamplerState = nullptr;
	}

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
	mShaderHandler.SetViewport(GameScreenManager::ScreenWidth, GameScreenManager::ScreenHeight, 0.0f, 1.0f, 0, 0);

	mShaderHandler.SetInputLayout(mShaderInputLayout);

	// Set the shaders we are going to use
	mShaderHandler.SetVertexShader(mVertexShader);
	mShaderHandler.SetPixelShader(mPixelShader);

	// Bind the texture to the shader
	mShaderHandler.BindTextureToShaders(0, 1, &mShaderResourceViewPostProcessing);
	mShaderHandler.BindSamplerState(0, 1, &mTextureSamplerState);

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
	if (!mRenderTargetViewPostProcessing || !mDepthStencilViewPostProcessing)
		return;

	// Bind the render target and depth buffer
	mShaderHandler.SetRenderTargets(1, &mRenderTargetViewPostProcessing, mDepthStencilViewPostProcessing);
}

// ------------------------------------------------------------------ 

void PostProcessing::ClearRenderTarget()
{
	if (!mRenderTargetViewPostProcessing || !mDepthStencilViewPostProcessing)
		return;

	mShaderHandler.ClearRenderTargetView(mRenderTargetViewPostProcessing, Constants::COLOUR_BLACK);
	mShaderHandler.ClearDepthStencilView(mDepthStencilViewPostProcessing, D3D11_CLEAR_DEPTH, 1.0f, 0);
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
	// ------------------------------------------------------------------------------------------------- //

	// Colour buffer setup

	// First create the texture
	D3D11_TEXTURE2D_DESC desc;
	desc.Width              = GameScreenManager::ScreenWidth;
	desc.Height             = GameScreenManager::ScreenHeight;
	desc.MipLevels          = 1;
	desc.ArraySize          = 1;
	desc.Format             = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count   = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage              = D3D11_USAGE_DEFAULT;
	desc.BindFlags          = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags     = 0;
	desc.MiscFlags          = 0;

	if (!mShaderHandler.CreateTexture2D(&desc, nullptr, &mPostProcessingTexture))
	{
		return;
	}

	// Now create the render target view through a description
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDescription;
	renderTargetViewDescription.Format             = DXGI_FORMAT_R32G32B32A32_FLOAT;
	renderTargetViewDescription.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDescription.Texture2D.MipSlice = 0;

	// This should return back the render target view into the final parameter
	mShaderHandler.CreateRenderTargetView(mPostProcessingTexture, &renderTargetViewDescription, &mRenderTargetViewPostProcessing);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format                    = DXGI_FORMAT_R32G32B32A32_FLOAT;
	shaderResourceViewDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels       = 1;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

	// This should return back the shader resource view into the final parameter
	mShaderHandler.CreateShaderResourceView(mPostProcessingTexture, &shaderResourceViewDesc, &mShaderResourceViewPostProcessing);

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

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc = shaderResourceViewDesc;
	resourceDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

	// Now create the shader resource from it - so that we can pass it into shaders later on
	mShaderHandler.CreateShaderResourceView(tempDepthTexture, &resourceDesc, &mDepthTextureResourceView);

	// Release the temp texture's memory 
	tempDepthTexture->Release();
	tempDepthTexture = nullptr;

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter     = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
	samplerDesc.AddressU   = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV   = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW   = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MinLOD     = 0;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

	mShaderHandler.CreateSamplerState(&samplerDesc, &mTextureSamplerState);
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