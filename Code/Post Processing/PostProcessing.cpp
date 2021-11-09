#include "PostProcessing.h"
#include "../Maths/Commons.h"
#include "../Maths/Constants.h"
#include "../GameScreens/ScreenManager.h"

// ------------------------------------------------------------------ 

PostProcessing::PostProcessing(ShaderHandler& shaderHandler)
	: mClouds(nullptr),
	  mPixelShader(nullptr),
	  mVertexShader(nullptr),
	  mShaderHandler(shaderHandler),
	  mShaderInputLayout(nullptr),
	  mIndexBuffer(nullptr),
	  mPostProcessingRenderBuffer(nullptr),
	  mSamplerState(nullptr),
	  mDepthStencilBuffer(nullptr)
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

	delete mDepthStencilBuffer;
	mDepthStencilBuffer = nullptr;

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

	// Unbind the texture
	mShaderHandler.UnbindTextureFromShaderIndex(0, 1);

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
	//RenderVolumetricClouds();



	// Render the post processed scene to the screen
	RenderFinal();
}

// ------------------------------------------------------------------ 

void PostProcessing::BindRenderTarget()
{
	if (!mPostProcessingRenderBuffer || !mDepthStencilBuffer)
		return;

	// Bind the render target and depth buffer
	mPostProcessingRenderBuffer->BindRenderTargetAsActive(mDepthStencilBuffer->GetDepthStencilView());
}

// ------------------------------------------------------------------ 

void PostProcessing::ClearRenderTarget()
{
	if (!mPostProcessingRenderBuffer || !mDepthStencilBuffer)
		return;

	mPostProcessingRenderBuffer->ClearRenderBuffer(mDepthStencilBuffer->GetDepthStencilView());
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
	// Create the render buffer
	mPostProcessingRenderBuffer = new RenderBuffer(mShaderHandler, GameScreenManager::ScreenWidth, GameScreenManager::ScreenHeight, 1, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_USAGE_DEFAULT);

	// Create the depth stencil buffer
	mDepthStencilBuffer = new DepthStencilBuffer(mShaderHandler, GameScreenManager::ScreenWidth, GameScreenManager::ScreenHeight, 1, 1, DXGI_FORMAT_R24G8_TYPELESS, D3D11_USAGE_DEFAULT, DXGI_FORMAT_D24_UNORM_S8_UINT, true, DXGI_FORMAT_R24_UNORM_X8_TYPELESS);

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