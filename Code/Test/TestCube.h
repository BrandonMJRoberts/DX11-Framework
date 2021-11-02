#ifndef _TEST_CUBE_H_
#define _TEST_CUBE_H_

#include "../Shaders/ShaderHandler.h"
#include "../Maths/CommonMaths.h"

class BaseCamera;

// ------------------------------------------------------------- //

class TestCube
{
public:
	TestCube(ShaderHandler& shaderHandler, Vector3D position);
	~TestCube();

	void Render(BaseCamera* camera);
	void Update(const float deltaTime);

	void move(const float deltaTime);

private:
	ShaderHandler&     mShaderHandler;

	ID3D11PixelShader*  mPixelShader;
	ID3D11VertexShader* mVertexShader;

	ID3D11Buffer*       mVertexBuffer;
	ID3D11Buffer*       mIndexBuffer;
	ID3D11Buffer*       mConstantBuffer;

	Vector3D            mPosition;

	DirectX::XMFLOAT4X4 modelMat;
};

// ------------------------------------------------------------- //

#endif