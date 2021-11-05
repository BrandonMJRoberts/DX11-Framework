#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <d3d11_1.h>
#include <d3d9types.h>

#include <string>

#include "../Shaders/ShaderHandler.h"

// ---------------------------------------------------------------- //

enum class TextureStates
{

};

// ---------------------------------------------------------------- //

class Texture2D final
{
public:
	Texture2D();
	Texture2D(ShaderHandler& shaderHandler, float width, float height, unsigned int mipLevels, unsigned int arraySize, D3D11_USAGE usage, unsigned int bindFlags, DXGI_FORMAT internalFormat);
	~Texture2D();

	void LoadTextureInFromFile(std::string& filePath);

	ID3D11Texture2D* GetInternalTexture() { return mInternalTexture; }

private:
	ID3D11Texture2D* mInternalTexture;
};

// ---------------------------------------------------------------- //

class Texture3D final
{
public:
	Texture3D();
	Texture3D(ShaderHandler& shaderHandler, float width, float height, float depth, unsigned int mipLevels, unsigned int arraySize, D3D11_USAGE usage, unsigned int bindFlags, DXGI_FORMAT internalFormat);
	~Texture3D();

	void LoadTextureInFromFile(std::string& filePath);

	ID3D11Texture3D* GetInternalTexture() { return mInternalTexture; }

private:
	ID3D11Texture3D* mInternalTexture;
};

// ---------------------------------------------------------------- //

#endif