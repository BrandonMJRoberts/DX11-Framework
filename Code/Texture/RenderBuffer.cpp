#include "RenderBuffer.h"

#include "../Maths/Constants.h"

// ------------------------------------------------------------------ //

RenderBuffer::RenderBuffer(ShaderHandler& shaderHandler, unsigned int width, unsigned int height, unsigned int mipLevels, unsigned int arraySize, DXGI_FORMAT format, D3D11_USAGE usage)
	: mTexture2D(nullptr)
	, mRenderTargetView(nullptr)
	, mShaderResourceView(nullptr)
	, mShaderHandler(shaderHandler)
{
	// First create the texture
	mTexture2D = new Texture2D(shaderHandler, width, height, mipLevels, arraySize, usage, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, format);

	// Now create the render target view
	D3D11_RENDER_TARGET_VIEW_DESC desc;
	desc.Format             = format;
	desc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;

	if (!shaderHandler.CreateRenderTargetView(mTexture2D->GetInternalTexture(), &desc, &mRenderTargetView))
		return;

	// Now create the shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format                    = format;
	shaderResourceViewDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels       = 1;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

	if (!shaderHandler.CreateShaderResourceView(mTexture2D->GetInternalTexture(), &shaderResourceViewDesc, &mShaderResourceView))
		return;
}

// ------------------------------------------------------------------ //

RenderBuffer::~RenderBuffer()
{
	delete mTexture2D;
	mTexture2D = nullptr;

	if (mRenderTargetView)
	{
		mRenderTargetView->Release();
		mRenderTargetView = nullptr;
	}

	if (mShaderResourceView)
	{
		mShaderResourceView->Release();
		mShaderResourceView = nullptr;
	}
}

// ------------------------------------------------------------------ //

void RenderBuffer::BindTextureToShaders(unsigned int startSlot, unsigned int numberOfViews)
{
	if (!mShaderResourceView)
		return;

	// Bind the texture to the active shaders
	mShaderHandler.BindTextureToShaders(startSlot, numberOfViews, &mShaderResourceView);
}

// ------------------------------------------------------------------ //

void RenderBuffer::BindRenderTargetAsActive(ID3D11DepthStencilView* depthStencilViewToBind)
{
	if (!mRenderTargetView)
		return;

	// Bind it as the active one
	mShaderHandler.SetRenderTargets(1, &mRenderTargetView, depthStencilViewToBind);
}

// ------------------------------------------------------------------ //

void RenderBuffer::ClearRenderBuffer(ID3D11DepthStencilView* depthStencilViewToClear)
{
	if(mRenderTargetView)
		mShaderHandler.ClearRenderTargetView(mRenderTargetView,      Constants::COLOUR_BLACK);

	if(depthStencilViewToClear)
		mShaderHandler.ClearDepthStencilView(depthStencilViewToClear, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

// ------------------------------------------------------------------ //
// Depth stencil buffer
// ------------------------------------------------------------------ // 


DepthStencilBuffer::DepthStencilBuffer(ShaderHandler& shaderHandler, 
	                                   unsigned int   width, 
	                                   unsigned int   height,
	                                   unsigned int   mipLevels, 
	                                   unsigned int   arraySize, 
	                                   DXGI_FORMAT    format, 
	                                   D3D11_USAGE    usage,
	                                   DXGI_FORMAT    depthStencilViewFormat,
	                                   bool           isShaderResource,
                                       DXGI_FORMAT    shaderResourceViewFormat)
	: mTexture2D(nullptr)
	, mDepthStencilView(nullptr)
	, mShaderResourceView(nullptr)
	, mIsShaderResource(isShaderResource)
{
	// Create the texture
	unsigned int flags = D3D11_BIND_DEPTH_STENCIL;

	if (mIsShaderResource)
		flags |= D3D11_BIND_SHADER_RESOURCE;

	mTexture2D = new Texture2D(shaderHandler, width, height, mipLevels, arraySize, usage, flags, format);

	if (!mTexture2D)
		return;

	// Now create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc;
	depthDesc.Format             = depthStencilViewFormat;
	depthDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthDesc.Flags              = 0;
	depthDesc.Texture2D.MipSlice = 0;

	if (!shaderHandler.CreateDepthStencilView(mTexture2D->GetInternalTexture(), &depthDesc, &mDepthStencilView))
		return;

	if(mIsShaderResource)
	{
		// Now create the shader resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format                    = shaderResourceViewFormat;
		shaderResourceViewDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MipLevels       = 1;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

		if (!shaderHandler.CreateShaderResourceView(mTexture2D->GetInternalTexture(), &shaderResourceViewDesc, &mShaderResourceView))
			return;
	}
}

// ------------------------------------------------------------------ // 

DepthStencilBuffer::~DepthStencilBuffer()
{
	delete mTexture2D;
	mTexture2D = nullptr;

	if (mDepthStencilView)
	{
		mDepthStencilView->Release();
		mDepthStencilView = nullptr;
	}

	if (mShaderResourceView)
	{
		mShaderResourceView->Release();
		mShaderResourceView = nullptr;
	}
}

// ------------------------------------------------------------------ // 