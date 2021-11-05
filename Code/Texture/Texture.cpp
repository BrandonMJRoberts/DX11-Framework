#include "Texture.h"

// --------------------------------------------------------------------- //
// TEXTURE 2D
// --------------------------------------------------------------------- //

Texture2D::Texture2D()
	: mInternalTexture(nullptr)
{

}

// --------------------------------------------------------------------- //

Texture2D::Texture2D(ShaderHandler& shaderHandler, float width, float height, unsigned int mipLevels, unsigned int arraySize, D3D11_USAGE usage, unsigned int bindFlags, DXGI_FORMAT internalFormat)
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

Texture3D::Texture3D()
	: mInternalTexture(nullptr)
{

}

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

Texture3D::Texture3D(ShaderHandler& shaderHandler, float width, float height, float depth, unsigned int mipLevels, unsigned int arraySize, D3D11_USAGE usage, unsigned int bindFlags, DXGI_FORMAT internalFormat)
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