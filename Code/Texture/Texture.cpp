#include "Texture.h"

// --------------------------------------------------------------------- //
// TEXTURE 2D
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //

Texture2D::Texture2D(ShaderHandler& shaderHandler, unsigned int width, unsigned int height, unsigned int mipLevels, unsigned int arraySize, D3D11_USAGE usage, unsigned int bindFlags, DXGI_FORMAT internalFormat, DXGI_FORMAT shaderResourceViewFormat)
	: mInternalTexture(nullptr),
	  mShaderResource(false),
	  mShaderResourceView(nullptr),
	  mShaderHandler(shaderHandler)
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
	if (!shaderHandler.CreateTexture2D(&desc, nullptr, &mInternalTexture))
		return;

	// Store if the texture can be used as a shader resource
	mShaderResource  = bindFlags & D3D11_BIND_SHADER_RESOURCE;

	// If we need to create a shader resource view for this texture then do so
	if (mShaderResource)
	{
		// Now create the shader resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format                    = shaderResourceViewFormat;
		shaderResourceViewDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MipLevels       = 1;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

		if (!mShaderHandler.CreateShaderResourceView(mInternalTexture, &shaderResourceViewDesc, &mShaderResourceView))
			return;
	}
}

// --------------------------------------------------------------------- //

Texture2D::~Texture2D()
{
	if (mInternalTexture)
	{
		mInternalTexture->Release();
		mInternalTexture = nullptr;
	}

	if (mShaderResourceView)
	{
		mShaderResourceView->Release();
		mShaderResourceView = nullptr;
	}
}

// --------------------------------------------------------------------- //

void Texture2D::LoadTextureInFromFile(std::string& filePath)
{

}

// --------------------------------------------------------------------- //

void Texture2D::BindTextureToShaders(unsigned int startSlot, unsigned int numberOfViews)
{
	// If have the functionality to bind to the shaders
	if (mShaderResource && mShaderResourceView)
	{
		mShaderHandler.BindTextureToShaders(startSlot, numberOfViews, &mShaderResourceView);
	}
}

// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// TEXTURE 3D
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //

Texture3D::Texture3D(ShaderHandler& shaderHandler, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevels, unsigned int arraySize, D3D11_USAGE usage, unsigned int bindFlags, DXGI_FORMAT internalFormat)
	: mInternalTexture(nullptr),
	  mShaderResource(false),
	  mShaderResourceView(nullptr),
	  mShaderHandler(shaderHandler)
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
	if (!shaderHandler.CreateTexture3D(&desc, nullptr, &mInternalTexture))
		return;

	// Store if the texture can be used as a shader resource
	mShaderResource  = bindFlags & D3D11_BIND_SHADER_RESOURCE;

	// If we need to create a shader resource view for this texture then do so
	if (mShaderResource)
	{
		// Now create the shader resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format                    = internalFormat;
		shaderResourceViewDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE3D;
		shaderResourceViewDesc.Texture2D.MipLevels       = 1;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

		if (!mShaderHandler.CreateShaderResourceView(mInternalTexture, &shaderResourceViewDesc, &mShaderResourceView))
			return;
	}
}

// --------------------------------------------------------------------- //

Texture3D::~Texture3D()
{
	if (mInternalTexture)
	{
		mInternalTexture->Release();
		mInternalTexture = nullptr;
	}

	if (mShaderResourceView)
	{
		mShaderResourceView->Release();
		mShaderResourceView = nullptr;
	}
}

// --------------------------------------------------------------------- //

void Texture3D::LoadTextureInFromFile(std::string& filePath)
{

}

// --------------------------------------------------------------------- //
// Sampler State
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //

SamplerState::SamplerState(ShaderHandler&             shaderHandler,
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

	if (!shaderHandler.CreateSamplerState(&samplerDesc, &mInternalState))
		return;
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