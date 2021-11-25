#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <d3d11_1.h>
#include <d3d9types.h>

#include <wincodec.h> // Texture loader built into directX

#include <string>

#include "../Shaders/ShaderHandler.h"

// ---------------------------------------------------------------- //

// Needed conversion functions
static DXGI_FORMAT        GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID);
static int                GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat);
static WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID);

class Texture2D final
{
public:
	Texture2D() = delete;
	Texture2D(ShaderHandler& shaderHandler);
	Texture2D(ShaderHandler& shaderHandler, unsigned int width, unsigned int height, unsigned int mipLevels, unsigned int arraySize, D3D11_USAGE usage, unsigned int bindFlags, DXGI_FORMAT internalFormat, DXGI_FORMAT shaderResourceViewFormat);
	~Texture2D();

	void LoadTextureInFromFile(LPCWSTR filePath);

	void BindTextureToShaders(unsigned int startSlot, unsigned int numberOfViews);

	ID3D11Texture2D* GetInternalTexture() { return mInternalTexture; }
	
	ID3D11UnorderedAccessView* GetUnorderedAccessView() { return mUnorderedAccessView; }

private:
	// Windows Imaging Context
	static IWICImagingFactory* mImagingFactory;

	ID3D11Texture2D*          mInternalTexture;

	ShaderHandler&            mShaderHandler;

	ID3D11ShaderResourceView* mShaderResourceView;
	bool                      mShaderResource;

	ID3D11UnorderedAccessView* mUnorderedAccessView;
};

// ---------------------------------------------------------------- //

class Texture3D final
{
public:
	Texture3D() = delete;
	Texture3D(ShaderHandler& shaderHandler, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevels, unsigned int arraySize, D3D11_USAGE usage, unsigned int bindFlags, DXGI_FORMAT internalFormat);
	~Texture3D();

	void LoadTextureInFromFile(std::string& filePath);

	void BindTextureToShaders(unsigned int startSlot, unsigned int numberOfViews);

	void BindTextureToComputeShader(unsigned int startSlot, unsigned int numberOfViews);
	void UnbindTextureFromComputeShader(unsigned int startSlot);

	ID3D11Texture3D*           GetInternalTexture() { return mInternalTexture; }
	ID3D11UnorderedAccessView* GetUnorderedAccessView() { return mUnorderedAccessView; }

private:
	ID3D11Texture3D*           mInternalTexture;

	ShaderHandler&             mShaderHandler;

	ID3D11ShaderResourceView*  mShaderResourceView;
	bool                       mShaderResource;

	ID3D11UnorderedAccessView* mUnorderedAccessView;
};

// ---------------------------------------------------------------- //

class SamplerState final
{
public:
	SamplerState() = delete;
	SamplerState(ShaderHandler&             shaderHandler,
		         D3D11_FILTER               filter, 
		         D3D11_TEXTURE_ADDRESS_MODE wrapU, 
		         D3D11_TEXTURE_ADDRESS_MODE wrapV, 
		         D3D11_TEXTURE_ADDRESS_MODE wrapW, 
		         float                      mipLODBias,
		         unsigned int               minLOD,
		         unsigned int               maxAnisotropy,
		         float                      borderColour0,
		         float                      borderColour1, 
		         float                      borderColour2, 
		         float                      borderColour3, 
		         D3D11_COMPARISON_FUNC      comparisonFunction);
	~SamplerState();

	void BindSamplerState(unsigned int startSlot, unsigned int count);

private:
	ID3D11SamplerState* mInternalState;

	ShaderHandler& mShaderHandler;
};

#endif