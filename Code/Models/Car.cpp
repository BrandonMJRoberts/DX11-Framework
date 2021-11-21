#include "Car.h"

// ------------------------------------------------------------------ //

Car::Car(ShaderHandler& shaderHandler)
	: mCarModel(nullptr)
	, mFullRenderVertexShader(nullptr)
	, mFullRenderPixelShader(nullptr)
	, mGeometryVertexShader(nullptr)
	, mGeometryPixelShader(nullptr)
	, mShaderHandler(shaderHandler)
	, mModelMat(MatrixMaths::Identity4X4)
{
	// Load in the shaders and then construct the model
	VertexShaderReturnData returnDataFullRender = mShaderHandler.CompileVertexShader(L"ModelLightingRender.fx", "VS");
	                    mFullRenderVertexShader = returnDataFullRender.vertexShader;

			            mFullRenderPixelShader  = mShaderHandler.CompilePixelShader(L"ModelLightingRender.fx", "PS");

    // Now load in the model
	mCarModel = new Model(mShaderHandler, "Models/Car/Car.obj", mFullRenderVertexShader, mFullRenderPixelShader, returnDataFullRender.Blob);
}

// ------------------------------------------------------------------ //

Car::~Car()
{
	delete mCarModel;
	mCarModel = nullptr;

	if (mFullRenderVertexShader)
	{
		mFullRenderVertexShader->Release();
		mFullRenderVertexShader = nullptr;
	}

	if (mFullRenderPixelShader)
	{
		mFullRenderPixelShader->Release();
		mFullRenderPixelShader = nullptr;
	}

	if (mGeometryPixelShader)
	{
		mGeometryPixelShader->Release();
		mGeometryPixelShader = nullptr;
	}

	if (mGeometryVertexShader)
	{
		mGeometryVertexShader->Release();
		mGeometryVertexShader = nullptr;
	}
}

// ------------------------------------------------------------------ //

void Car::RenderFull(BaseCamera* camera)
{
	if (mCarModel)
	{
		mCarModel->FullRender(camera, mModelMat);
	}
}

// ------------------------------------------------------------------ //

void Car::Update(const float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);
}

// ------------------------------------------------------------------ //