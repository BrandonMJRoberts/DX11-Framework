#ifndef _MODEL_H_
#define _MODEL_H_

#include <string>

#include <d3d11_1.h>
#include <directxmath.h>

#include "../Maths/CommonMaths.h"

// -------------------------------------------------------------- //

struct VertexData final
{
	VertexData()
	{
		vertexPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		normal         = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		textureCoord   = DirectX::XMFLOAT2(0.0f, 0.0f);
	}

	DirectX::XMFLOAT3 vertexPosition;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 textureCoord;
};

struct FaceData final
{
	FaceData()
	{
		verticies[0] = VertexData();
		verticies[1] = VertexData();
		verticies[2] = VertexData();
	}

	VertexData verticies[3];
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

	Vector3D ExtractThreeDataPointsFromLine(std::string& line);
	Vector2D ExtractTwoDataPointsFromLine(std::string& line);

	FaceData ConstructFaceFromData(std::string& line, Vector3D* vertexData, Vector3D* normalData, Vector2D* textureCoordData);

	void     ExtractFaceIndexDataFromFile(std::string& line, Vector3D indexDataFromFile[3]);

	// Vertex and index data
	FaceData* mFaceData;
};

// -------------------------------------------------------------- //

#endif