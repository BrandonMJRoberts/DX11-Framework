#include "Texture.h"

// -----------------------------------------------------------------------------------

// Converter function from WIC format to DXGI format
DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID)
{
	if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFloat)    return DXGI_FORMAT_R32G32B32A32_FLOAT;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAHalf) return DXGI_FORMAT_R16G16B16A16_FLOAT;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBA)     return DXGI_FORMAT_R16G16B16A16_UNORM;
    else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA)     return DXGI_FORMAT_R8G8B8A8_UNORM;
    else if (wicFormatGUID == GUID_WICPixelFormat32bppBGRA)     return DXGI_FORMAT_B8G8R8A8_UNORM;
    else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR)      return DXGI_FORMAT_B8G8R8X8_UNORM;
    else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102XR) return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;

    else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102) return DXGI_FORMAT_R10G10B10A2_UNORM;
    else if (wicFormatGUID == GUID_WICPixelFormat16bppBGRA5551)    return DXGI_FORMAT_B5G5R5A1_UNORM;
    else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR565)      return DXGI_FORMAT_B5G6R5_UNORM;
    else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFloat)   return DXGI_FORMAT_R32_FLOAT;
    else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayHalf)    return DXGI_FORMAT_R16_FLOAT;
    else if (wicFormatGUID == GUID_WICPixelFormat16bppGray)        return DXGI_FORMAT_R16_UNORM;
    else if (wicFormatGUID == GUID_WICPixelFormat8bppGray)         return DXGI_FORMAT_R8_UNORM;
    else if (wicFormatGUID == GUID_WICPixelFormat8bppAlpha)        return DXGI_FORMAT_A8_UNORM;

    else return DXGI_FORMAT_UNKNOWN;
}

// -----------------------------------------------------------------------------------

int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat)
{
	if (dxgiFormat == DXGI_FORMAT_R32G32B32A32_FLOAT)      return 128;
    else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_FLOAT) return 64;
    else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_UNORM) return 64;
    else if (dxgiFormat == DXGI_FORMAT_R8G8B8A8_UNORM)     return 32;
    else if (dxgiFormat == DXGI_FORMAT_B8G8R8A8_UNORM)     return 32;
    else if (dxgiFormat == DXGI_FORMAT_B8G8R8X8_UNORM)     return 32;
    else if (dxgiFormat == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM) return 32;

    else if (dxgiFormat == DXGI_FORMAT_R10G10B10A2_UNORM) return 32;
    else if (dxgiFormat == DXGI_FORMAT_B5G5R5A1_UNORM)    return 16;
    else if (dxgiFormat == DXGI_FORMAT_B5G6R5_UNORM)	  return 16;
    else if (dxgiFormat == DXGI_FORMAT_R32_FLOAT)		  return 32;
    else if (dxgiFormat == DXGI_FORMAT_R16_FLOAT)		  return 16;
    else if (dxgiFormat == DXGI_FORMAT_R16_UNORM)		  return 16;
    else if (dxgiFormat == DXGI_FORMAT_R8_UNORM)		  return 8;
    else if (dxgiFormat == DXGI_FORMAT_A8_UNORM)		  return 8;

	return 8;
}

// -----------------------------------------------------------------------------------

WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID)
{
	if (wicFormatGUID == GUID_WICPixelFormatBlackWhite) return GUID_WICPixelFormat8bppGray;
    else if (wicFormatGUID == GUID_WICPixelFormat1bppIndexed) return GUID_WICPixelFormat32bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat2bppIndexed) return GUID_WICPixelFormat32bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat4bppIndexed) return GUID_WICPixelFormat32bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat8bppIndexed) return GUID_WICPixelFormat32bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat2bppGray) return GUID_WICPixelFormat8bppGray;
    else if (wicFormatGUID == GUID_WICPixelFormat4bppGray) return GUID_WICPixelFormat8bppGray;
    else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayFixedPoint) return GUID_WICPixelFormat16bppGrayHalf;
    else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFixedPoint) return GUID_WICPixelFormat32bppGrayFloat;
    else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR555) return GUID_WICPixelFormat16bppBGRA5551;
    else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR101010) return GUID_WICPixelFormat32bppRGBA1010102;
    else if (wicFormatGUID == GUID_WICPixelFormat24bppBGR) return GUID_WICPixelFormat32bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat24bppRGB) return GUID_WICPixelFormat32bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat32bppPBGRA) return GUID_WICPixelFormat32bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat32bppPRGBA) return GUID_WICPixelFormat32bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat48bppRGB) return GUID_WICPixelFormat64bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat48bppBGR) return GUID_WICPixelFormat64bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppBGRA) return GUID_WICPixelFormat64bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppPRGBA) return GUID_WICPixelFormat64bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppPBGRA) return GUID_WICPixelFormat64bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat48bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
    else if (wicFormatGUID == GUID_WICPixelFormat48bppBGRFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppBGRAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
    else if (wicFormatGUID == GUID_WICPixelFormat48bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
    else if (wicFormatGUID == GUID_WICPixelFormat128bppPRGBAFloat) return GUID_WICPixelFormat128bppRGBAFloat;
    else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBFloat) return GUID_WICPixelFormat128bppRGBAFloat;
    else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
    else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
    else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBE) return GUID_WICPixelFormat128bppRGBAFloat;
    else if (wicFormatGUID == GUID_WICPixelFormat32bppCMYK) return GUID_WICPixelFormat32bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppCMYK) return GUID_WICPixelFormat64bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat40bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat80bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
    else if (wicFormatGUID == GUID_WICPixelFormat32bppRGB) return GUID_WICPixelFormat32bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppRGB) return GUID_WICPixelFormat64bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppPRGBAHalf) return GUID_WICPixelFormat64bppRGBAHalf;
#endif

    else return GUID_WICPixelFormatDontCare;
}

// --------------------------------------------------------------------- //
// TEXTURE 2D
// --------------------------------------------------------------------- //

IWICImagingFactory* Texture2D::mImagingFactory = nullptr;

// --------------------------------------------------------------------- //

Texture2D::Texture2D(ShaderHandler& shaderHandler)
	: mInternalTexture(nullptr),
	  mShaderResource(false),
	  mShaderResourceView(nullptr),
	  mShaderHandler(shaderHandler)
{

}

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

void Texture2D::LoadTextureInFromFile(LPCWSTR filePath)
{
	IWICBitmapDecoder*     WICDecoder   = nullptr;
	IWICBitmapFrameDecode* WICFrame     = nullptr;
	IWICFormatConverter*   WICConverter = nullptr;

	bool imageConverted = false;

	HRESULT hr;

	// Initialise the COM library
	hr = CoInitialize(NULL);
	if (FAILED(hr))
		return;

	// Create the factory
	hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&mImagingFactory));
	if (FAILED(hr))
		return;

	// Load a decoder for this image
	hr = mImagingFactory->CreateDecoderFromFilename(filePath, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &WICDecoder);
	if (FAILED(hr))
		return;

	// Get the image from the decoder
	hr = WICDecoder->GetFrame(0, &WICFrame);
	if (FAILED(hr))
		return;

	// Get the WIC pixel format of the image
	WICPixelFormatGUID pixelFormat;
	hr = WICFrame->GetPixelFormat(&pixelFormat);
	if (FAILED(hr))
		return;

	// Now get the size of the image
	unsigned int textureWidth, textureHeight;
	hr = WICFrame->GetSize(&textureWidth, &textureHeight);
	if (FAILED(hr))
		return;

	// Convert WIC pixel format to the GXGI pixel format
	DXGI_FORMAT dxgiFormat = GetDXGIFormatFromWICFormat(pixelFormat);

	// If the format is known then try to convert it
	if (dxgiFormat == DXGI_FORMAT_UNKNOWN)
	{
		WICPixelFormatGUID convertToPixelFormat = GetConvertToWICFormat(pixelFormat);

		// Exit if no format that matches was found
		if (convertToPixelFormat == GUID_WICPixelFormatDontCare)
			return;

		// Set the DXGI format
		dxgiFormat = GetDXGIFormatFromWICFormat(convertToPixelFormat);

		// Create the format converter
		hr = mImagingFactory->CreateFormatConverter(&WICConverter);
		if(FAILED(hr))
			return;

		BOOL canConvert = false;
		hr = WICConverter->CanConvert(pixelFormat, convertToPixelFormat, &canConvert);
		if (FAILED(hr) || !canConvert)
			return;

		// Convert
		hr = WICConverter->Initialize(WICFrame, convertToPixelFormat, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
		if (FAILED(hr))
			return;

		imageConverted = true;
	}

	// Now get the amount of bits per pixel for the format
	int          bitsPerPixel = GetDXGIFormatBitsPerPixel(dxgiFormat);
	unsigned int bytesPerRow  = (textureWidth * bitsPerPixel) / 8;
	unsigned int imageSize    = bytesPerRow * textureHeight;

	// Now allocate the correct amount of data for this image
	char* imageData = new char[imageSize];

	if (imageConverted)
	{
		// Try to copy the pixels accross from the correct location
		hr = WICConverter->CopyPixels(0, bytesPerRow, imageSize, (BYTE*)imageData);

		if (FAILED(hr))
			return;
	}
	else
	{
		// Now copy the decoded raw data across into our store
		hr = WICFrame->CopyPixels(0, bytesPerRow, imageSize, (BYTE*)imageData);
		if (FAILED(hr))
			return;
	}

	// Now create the texture according to the data we have loaded in
	D3D11_TEXTURE2D_DESC desc;
	desc.Width              = textureWidth;
	desc.Height             = textureHeight;
	desc.MipLevels          = 1;
	desc.ArraySize          = 1;
	desc.Format             = dxgiFormat;
	desc.SampleDesc.Count   = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage              = D3D11_USAGE_DEFAULT;
	desc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags     = 0;
	desc.MiscFlags          = 0;

	// Create the texture
	D3D11_SUBRESOURCE_DATA initialData;
	initialData.pSysMem          = imageData;
	initialData.SysMemPitch      = bytesPerRow;
	initialData.SysMemSlicePitch = 0; // Bytes per depth row

	if (!mShaderHandler.CreateTexture2D(&desc, &initialData, &mInternalTexture))
		return;

	// Clear up the memory we have allocated
	delete[] imageData;
	imageData = nullptr;

	// If we are loading this from file then we are going to want to use it in shaders
	mShaderResource = true;
	if (mShaderResource)
	{
		// Now create the shader resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format                    = dxgiFormat;
		shaderResourceViewDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MipLevels       = 1;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

		if (!mShaderHandler.CreateShaderResourceView(mInternalTexture, &shaderResourceViewDesc, &mShaderResourceView))
			return;
	}
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
	samplerDesc.MinLOD         = (FLOAT)minLOD;
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