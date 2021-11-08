#ifndef _POST_PROCESSING_H_
#define _POST_PROCESSING_H_

#include <d3d11_1.h>
#include <d3d9types.h>

#include "../Shaders/ShaderHandler.h"

#include "../Texture/Texture.h"
#include "../Texture/RenderBuffer.h"

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

	RenderBuffer*             mPostProcessingRenderBuffer;
	DepthStencilBuffer*       mDepthStencilBuffer;

	// The volumetric clouds that are going to be rendered using the post processing system
	VolumetricClouds*       mClouds;

	// The pixel and vertex shaders that are going to be used for the final rendering to the screen
	ID3D11PixelShader*  mPixelShader;
	ID3D11VertexShader* mVertexShader;

	ID3D11InputLayout*  mShaderInputLayout;

	SamplerState*       mSamplerState;

	ID3D11Buffer*       mVertexBuffer;
	ID3D11Buffer*       mIndexBuffer;

	// The access to DirectX functionality
	ShaderHandler&      mShaderHandler;

	Texture2D* testImage;
};

#endif