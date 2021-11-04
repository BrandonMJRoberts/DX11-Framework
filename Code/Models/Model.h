#ifndef _MODEL_H_
#define _MODEL_H_

#include <string>

#include <d3d11_1.h>
#include <directxmath.h>

// -------------------------------------------------------------- //

struct VertexData final
{
	DirectX::XMFLOAT3 vertexPosition;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 textureCoord;
};

// -------------------------------------------------------------- //

class ShaderHandler;

// A model is an OBJ model as I am not supporting any other model types
class Model final
{
public:
	Model(ShaderHandler& shaderHandler, std::string filePathToLoadFrom = "");
	~Model();

	bool LoadInModelFromFile(std::string filePath);
	void RemoveAllPriorDataStored();

	// Render functionality
	void RenderGeometry();
	void FullRender();

private:
	ShaderHandler& mShaderHandler;

	// Vertex and index data
	VertexData*    mVertexData;
	unsigned int*  mIndexData;
};

// -------------------------------------------------------------- //

#endif