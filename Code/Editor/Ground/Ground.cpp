#include "Ground.h"

// ------------------------------------------------------- //

Ground::Ground(ShaderHandler& shaderHandler)
	: mShaderHandler(shaderHandler)
	, mGroundModel(nullptr)
	, mVertexShader(nullptr)
	, mPixelShader(nullptr)
{
	// First load in the shaders
	VertexShaderReturnData returnData = mShaderHandler.CompileVertexShader(L"ModelLightingRender.fx", "VS");
	                    mVertexShader = returnData.vertexShader;

						mPixelShader = mShaderHandler.CompilePixelShader(L"ModelLightingRender.fx", "PS");

	// Now load in the model for the ground
	//mGroundModel = new Model(shaderHandler, "Models/Ground/Ground.obj", mVertexShader, mPixelShader, returnData.Blob);
}

// ------------------------------------------------------- //

Ground::~Ground()
{
	delete mGroundModel;
	mGroundModel = nullptr;
}

// ------------------------------------------------------- //

void Ground::Render(BaseCamera* camera)
{
	// Render the ground at the origin
	if (mGroundModel)
		mGroundModel->FullRender(camera, MatrixMaths::Identity4X4);
}

// ------------------------------------------------------- //

void Ground::Update(const float deltaTime)
{

}

// ------------------------------------------------------- //