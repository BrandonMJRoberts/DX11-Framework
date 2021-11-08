#ifndef _RENDER_BUFFER_H_
#define _RENDER_BUFFER_H_

#include "Texture.h"

// -------------------------------------------------------------------------------------------------- //

class RenderBuffer final
{
public:
	RenderBuffer() = delete;
	RenderBuffer(ShaderHandler& shaderHandler, unsigned int width, unsigned int height, unsigned int mipLevels, unsigned int arraySize, DXGI_FORMAT format, D3D11_USAGE usage);
	~RenderBuffer();

	void BindTextureToShaders(unsigned int startSlot, unsigned int numberOfViews);
	void BindRenderTargetAsActive(ID3D11DepthStencilView* depthStencilViewToBind = nullptr);

	void ClearRenderBuffer(ID3D11DepthStencilView* depthStencilViewToClear = nullptr);

private:
	// Texture - stores the actual data
	Texture2D* mTexture2D;

	// Render target view - used for binding things to render to this
	ID3D11RenderTargetView* mRenderTargetView;

	// Shader Resource view - used to pass the texture into shaders
	//ID3D11ShaderResourceView* mShaderResourceView;

	ShaderHandler& mShaderHandler;
};

// -------------------------------------------------------------------------------------------------- //

class DepthStencilBuffer final
{
public:
	DepthStencilBuffer() = delete;
	DepthStencilBuffer(ShaderHandler& shaderHandler, 
	                   unsigned int   width, 
	                   unsigned int   height,
	                   unsigned int   mipLevels, 
	                   unsigned int   arraySize, 
	                   DXGI_FORMAT    format, 
	                   D3D11_USAGE    usage,
	                   DXGI_FORMAT    depthStencilViewFormat,
		               bool           isShaderResource,
                       DXGI_FORMAT    shaderResourceViewFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS);
	~DepthStencilBuffer();

	ID3D11DepthStencilView* GetDepthStencilView() { return mDepthStencilView; }

	void BindTextureToShaders(unsigned int startSlot, unsigned int numberOfViews);

private:
	Texture2D* mTexture2D;

	// So it can be bound to render buffers
	ID3D11DepthStencilView* mDepthStencilView;

	// So it can be bound to shaders as a texture
	//ID3D11ShaderResourceView* mShaderResourceView;

	bool mIsShaderResource;
};

// -------------------------------------------------------------------------------------------------- //

#endif