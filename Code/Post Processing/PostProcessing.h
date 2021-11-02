#ifndef _POST_PROCESSING_H_
#define _POST_PROCESSING_H_

#include <d3d11_1.h>
#include <d3d9types.h>

#include "../Shaders/ShaderHandler.h"

#include "Clouds.h"

class PostProcessing final
{
public:
	PostProcessing(ShaderHandler& shaderHandler);
	~PostProcessing();

	void BindRenderTarget();
	void ClearRenderTarget();

	void Render();

private:
	// Final full screen render
	void RenderFinal();
	void RenderVolumetricClouds();

	void LoadInShaders();
	void SetupRenderTextures();
	void SetupBuffers();

	ID3D11Texture2D*          mPostProcessingTexture;            // The actual texture
	ID3D11RenderTargetView*   mRenderTargetViewPostProcessing;   // Used to bind the texture to be rendered to 
	ID3D11ShaderResourceView* mShaderResourceViewPostProcessing; // Used to pass the texture into shaders - this is rendered to the default render target in the final render

	ID3D11DepthStencilView*   mDepthStencilViewPostProcessing;   // Used to have things rendered to it (Depth and stencil data)
	ID3D11ShaderResourceView* mDepthTextureResourceView;         // Used to pass the depth texture into shaders as a texture

	// The volumetric clouds that are going to be rendered using the post processing system
	VolumetricClouds*       mClouds;

	// The pixel and vertex shaders that are going to be used for the final rendering to the screen
	ID3D11PixelShader*  mPixelShader;
	ID3D11VertexShader* mVertexShader;

	ID3D11InputLayout*  mShaderInputLayout;
	ID3D11SamplerState* mTextureSamplerState;

	ID3D11Buffer*       mVertexBuffer;
	ID3D11Buffer*       mIndexBuffer;

	// The access to DirectX functionality
	ShaderHandler&      mShaderHandler;
};

#endif