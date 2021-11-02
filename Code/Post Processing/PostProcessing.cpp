#include "PostProcessing.h"
#include "../Maths/Commons.h"
#include "../Maths/Constants.h"

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
	  mShaderHandler(shaderHandler)
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
}

// ------------------------------------------------------------------ 

void PostProcessing::RenderFinal()
{
	// Re-bind the default render target
	mShaderHandler.SetDefaultRenderTarget();

	// Set the shaders we are going to use
	mShaderHandler.SetVertexShader(mVertexShader);
	mShaderHandler.SetPixelShader(mPixelShader);

	// Bind the default render buffer for this render
	mShaderHandler.SetDefaultRenderTarget();

	// Bind the texture to the shader
	mShaderHandler.BindTextureToShaders(0, 1, &mShaderResourceViewPostProcessing);

	// Now draw the final screen
	mShaderHandler.Draw(6, 0);

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
	mShaderHandler.ClearDepthStencilView(mDepthStencilViewPostProcessing, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
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
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if (!mShaderHandler.SetDeviceInputLayout(returnData.Blob, layout, 2))
		return;
}

// ------------------------------------------------------------------ 

void PostProcessing::SetupRenderTextures()
{
	// ------------------------------------------------------------------------------------------------- //

	// Colour buffer setup

	// First create the texture
	D3D11_TEXTURE2D_DESC desc;
	desc.Width              = 1920;
	desc.Height             = 1080;
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
	renderTargetViewDescription.Format             = desc.Format;
	renderTargetViewDescription.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDescription.Texture2D.MipSlice = 0;

	// This should return back the render target view into the final parameter
	mShaderHandler.CreateRenderTargetView(mPostProcessingTexture, &renderTargetViewDescription, &mRenderTargetViewPostProcessing);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format                    = desc.Format;
	shaderResourceViewDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels       = 1;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

	// This should return back the shader resource view into the final parameter
	mShaderHandler.CreateShaderResourceView(mPostProcessingTexture, &shaderResourceViewDesc, &mShaderResourceViewPostProcessing);

	// ------------------------------------------------------------------------------------------------- //

	// Depth texture setup

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width              = 1920;
	depthStencilDesc.Height				= 1080;
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

	// Now create the shader resource from it - so that we can pass it into shaders later on
	mShaderHandler.CreateShaderResourceView(tempDepthTexture, &shaderResourceViewDesc, &mDepthTextureResourceView);

	// Release the temp texture's memory 
	tempDepthTexture->Release();
	tempDepthTexture = nullptr;
}

// ------------------------------------------------------------------ 

void PostProcessing::SetupBuffers()
{
	// Define the verticies we need for this to work
	TextureVertex verticies[] =
	{
		{DirectX::XMFLOAT3(-1.0f,  1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f)},
		{DirectX::XMFLOAT3( 1.0f,  1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f)},
		{DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f)},

		{DirectX::XMFLOAT3( 1.0f,  1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f)},
		{DirectX::XMFLOAT3( 1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f)},
		{DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f)}
	};

	ID3D11Buffer* vertexBuffer;

	// Create the vertex buffer
	if (!mShaderHandler.CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, (D3D11_CPU_ACCESS_FLAG)0, verticies, sizeof(TextureVertex) * 6, &vertexBuffer))
		return;

	UINT stride = sizeof(TextureVertex);
	UINT offset = 0;

	// Bind the vertex buffer to the correct registers
	if (!mShaderHandler.BindVertexBuffersToRegisters(0, 1, &vertexBuffer, &stride, &offset))
		return;
}

// ------------------------------------------------------------------ 