#ifndef _CAR_H_
#define _CAR_H_

#include "Model.h"

#include <d3d11_1.h>
#include <d3d9types.h>

#include "../Camera/ThirdPersonCamera.h"

class Car final
{
public:
	Car() = delete;
	Car(ShaderHandler& shaderHandler);
	~Car();

	void RenderFull(BaseCamera* camera);
	void RenderGeometry(BaseCamera* camera);

	void Update(const float deltaTime);

private:
	Model* mCarModel;

	// Full render
	ID3D11VertexShader* mFullRenderVertexShader;
	ID3D11PixelShader*  mFullRenderPixelShader;

	// Geometry only render
	ID3D11PixelShader*  mGeometryPixelShader;
	ID3D11VertexShader* mGeometryVertexShader;

	ShaderHandler& mShaderHandler;

	DirectX::XMFLOAT4X4 mModelMat;

};

#endif