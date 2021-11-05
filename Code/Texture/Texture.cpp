#include "Texture.h"

// --------------------------------------------------------------------- //
// TEXTURE 2D
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //

Texture2D::Texture2D(ShaderHandler& shaderHandler, unsigned int width, unsigned int height, unsigned int mipLevels, unsigned int arraySize, D3D11_USAGE usage, unsigned int bindFlags, DXGI_FORMAT internalFormat)
	: mInternalTexture(nullptr)
{
		D3D11_TEXTURE2D_DESC desc;
	desc.Width              = width;
	desc.Height             = height;
	desc.MipLevels          = mipLevels;
	desc.ArraySize          = arraySize;
	desc.Format             = internalFormat;
	desc.SampleDesc.Count   = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage              = usage;
	desc.BindFlags          = bindFlags;
	desc.CPUAccessFlags     = 0;
	desc.MiscFlags          = 0;

	// Create the texture
	shaderHandler.CreateTexture2D(&desc, nullptr, &mInternalTexture);
}

// --------------------------------------------------------------------- //

Texture2D::~Texture2D()
{
	if (mInternalTexture)
	{
		mInternalTexture->Release();
		mInternalTexture = nullptr;
	}
}

// --------------------------------------------------------------------- //

void Texture2D::LoadTextureInFromFile(std::string& filePath)
{

}

// --------------------------------------------------------------------- //
// TEXTURE 3D
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //

Texture3D::~Texture3D()
{
	if (mInternalTexture)
	{
		mInternalTexture->Release();
		mInternalTexture = nullptr;
	}
}

// --------------------------------------------------------------------- //

Texture3D::Texture3D(ShaderHandler& shaderHandler, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevels, unsigned int arraySize, D3D11_USAGE usage, unsigned int bindFlags, DXGI_FORMAT internalFormat)
	: mInternalTexture(nullptr)
{
	D3D11_TEXTURE3D_DESC desc;
	desc.Width			= width;
	desc.Height			= height;
	desc.Depth			= depth;
	desc.MipLevels		= mipLevels;
	desc.Format         = internalFormat;
	desc.BindFlags      = bindFlags;
	desc.CPUAccessFlags = 0;
	desc.CPUAccessFlags = 0;
	desc.Usage          = usage;

	// Create the texture
	shaderHandler.CreateTexture3D(&desc, nullptr, &mInternalTexture);
}

// --------------------------------------------------------------------- //

void Texture3D::LoadTextureInFromFile(std::string& filePath)
{

}

// --------------------------------------------------------------------- //
// Sampler State
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //

SamplerState::SamplerState(ShaderHandler              shaderHandler,
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
						   D3D11_COMPARISON_FUNC      comparisonFunction)
	: mInternalState(nullptr)
	, mShaderHandler(shaderHandler)
{
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter         = filter;
	samplerDesc.AddressU       = wrapU;
	samplerDesc.AddressV       = wrapV;
	samplerDesc.AddressW       = wrapW;
	samplerDesc.MipLODBias     = mipLODBias;
	samplerDesc.MinLOD         = minLOD;
	samplerDesc.MaxAnisotropy  = maxAnisotropy;
	samplerDesc.BorderColor[0] = borderColour0;
	samplerDesc.BorderColor[1] = borderColour1;
	samplerDesc.BorderColor[2] = borderColour2;
	samplerDesc.BorderColor[3] = borderColour3;
	samplerDesc.ComparisonFunc = comparisonFunction;

	shaderHandler.CreateSamplerState(&samplerDesc, &mInternalState);
}

// --------------------------------------------------------------------- //

SamplerState::~SamplerState()
{
	if (mInternalState)
	{
		mInternalState->Release();
		mInternalState = nullptr;
	}
}

// --------------------------------------------------------------------- //

void SamplerState::BindSamplerState(unsigned int startSlot, unsigned int count)
{
	if (!mInternalState)
		return;

	mShaderHandler.BindSamplerState(startSlot, count, &mInternalState);
}

// --------------------------------------------------------------------- //