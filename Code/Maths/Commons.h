#pragma once

// -----------------------------------------------------------------------------

// Vertex types
struct SimpleVertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 colour;
};

struct TextureVertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 textureCoords;
};

// -----------------------------------------------------------------------------

// Constant buffer types
struct ConstantBuffer final
{
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;
	Vector3D          cameraPosition;
	float             padding;
};

// -----------------------------------------------------------------------------

struct BasicDirectionalLightingData final
{
	DirectX::XMFLOAT4 colour;
	DirectX::XMFLOAT3 direction;
	float padding;
};

// -----------------------------------------------------------------------------

struct BasicMaterialData final
{
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;
	float             shinness;
	Vector3D          padding;
};

// -----------------------------------------------------------------------------