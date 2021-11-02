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
struct ConstantBuffer
{
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;
};

// -----------------------------------------------------------------------------