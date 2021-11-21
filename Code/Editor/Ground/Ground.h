#ifndef _WORLD_GROUND_H_
#define _WORLD_GROUND_H_

#include "../../Models/Model.h"
#include "../../Camera/BaseCamera.h"
#include "../../Shaders/ShaderHandler.h"

class Ground final
{
public:
	Ground(ShaderHandler& shaderHandler);
	~Ground();

	void Render(BaseCamera* camera);
	void Update(const float deltaTime);

private:
	Model*              mGroundModel;

	ID3D11VertexShader* mVertexShader;
	ID3D11PixelShader*  mPixelShader;

	ShaderHandler&      mShaderHandler;
};

#endif