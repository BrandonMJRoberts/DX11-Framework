#ifndef _MODEL_H_
#define _MODEL_H_

#include <string>
#include <vector>

#include <d3d11_1.h>
#include <directxmath.h>

#include "../Maths/CommonMaths.h"
#include "../Texture/Texture.h"

#include "../Camera/BaseCamera.h"

// -------------------------------------------------------------- //

struct VertexData final
{
	VertexData()
	{
		vertexPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		textureCoord   = DirectX::XMFLOAT2(0.0f, 0.0f);
		normal         = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	}

	DirectX::XMFLOAT3 vertexPosition;
	DirectX::XMFLOAT2 textureCoord;
	DirectX::XMFLOAT3 normal;
};

struct MaterialData final
{
	MaterialData()
		: specularPower(1)
		, ambient(1.0f, 1.0f, 1.0f, 1.0f)
		, specular(1.0f, 1.0f, 1.0f, 1.0f)
		, diffuse(1.0f, 1.0f, 1.0f, 1.0f)
		//, texture(nullptr)
	{

	}

	~MaterialData() 
	{
//		delete texture;
		//texture = nullptr;
	}

	Vector4D   ambient;
	Vector4D   diffuse;
	Vector4D   specular;
	float      specularPower;
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
	Model(ShaderHandler&         shaderHandler, 
	         std::string         filePathToLoadFrom, 

	         ID3D11VertexShader* fullRenderVertexShader, 
	         ID3D11PixelShader*  fullRenderPixelShader,
	         ID3DBlob*           fullRenderBlob);
	~Model();

	bool LoadInModelFromFile(std::string filePath);
	void RemoveAllPriorDataStored();

	// Render functionality
	void FullRender(BaseCamera* camera, const DirectX::XMFLOAT4X4 modelMat);

	void SetShadersForFullRender(ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader);

private:
	ShaderHandler& mShaderHandler;

	// .obj loader helper functions
	Vector3D ExtractThreeDataPointsFromLine(std::string& line);
	Vector2D ExtractTwoDataPointsFromLine(std::string& line);

	void     ConstructFaceFromData(std::string& line, Vector3D* vertexData, Vector3D* normalData, Vector2D* textureCoordData, FaceData& faceBeingReturned);

	void     ExtractFaceIndexDataFromFile(std::string& line, Vector3D indexDataFromFile[3]);

	void     SetupInputLayouts(ID3DBlob* fullRenderBlob);

	void     LoadInMaterialData(std::string filePath);

	// Internal model data - stored as faces, each holding three verticies each
	FaceData* mFaceData;

	// Shaders used to render the model - are passed in either when the model is created, or at some later point
	ID3D11VertexShader* mFullRenderVertexShader;
	ID3D11PixelShader*  mFullRenderPixelShader;

	ID3D11Buffer*       mVertexBuffer;
	unsigned int        mVertexBufferStride;
	unsigned int        mVertexBufferOffset;
	unsigned int        mVertexCount;

	ID3D11Buffer*       mMatrixConstantBuffer;
	ID3D11Buffer*       mMaterialConstantBuffer;
	ID3D11Buffer*       mLightConstantBuffer;

	ID3D11InputLayout*  mFullRenderInputLayout;

	std::vector<MaterialData*> mMaterialData;
	Texture2D*                 placeHolderTexture;

	SamplerState*              mSamplerState;
};

// -------------------------------------------------------------- //

#endif