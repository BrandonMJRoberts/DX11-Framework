#ifndef _SKY_H_
#define _SKY_H_

#include "../Camera/BaseCamera.h"
#include "../Maths/CommonMaths.h"

#include <d3d11_1.h>

#include "../Shaders/ShaderHandler.h"

class SkyDome final
{
public:
	SkyDome() = delete;
	SkyDome(ShaderHandler& shaderHandler, Vector3D centre, float radius, unsigned int slicesX, unsigned int slicesY);
	~SkyDome();

	void Render(BaseCamera* camera);
	void Update(const float deltaTime);

private:
	void GenerateDome(Vector3D centre, float radius, unsigned int slicesX, unsigned int slicesY);
	void SetupShaders();

	Vector3D     mDomeCentre;
	float        mRadius;
	unsigned int mSlicesX;
	unsigned int mSlicesY;
	
	// Shaders required
	ID3D11VertexShader* mVertexShader;
	ID3D11PixelShader*  mPixelShader;

	// Buffer holding the vertex data for the dome
	ID3D11Buffer*       mVertexBuffer;
	ID3D11Buffer*       mIndexBuffer;
	ID3D11Buffer*       mConstantBuffer;

	Vector3D*           mVertexData;
	short*       mIndexData;

	unsigned int        mVertexBufferStride;
	unsigned int        mVertexCount;
	unsigned int        mIndexCount;

	ShaderHandler&      mShaderHandler;

	ID3D11InputLayout*  mInputLayout;

	DirectX::XMFLOAT4X4 mModelMat;
};

#endif