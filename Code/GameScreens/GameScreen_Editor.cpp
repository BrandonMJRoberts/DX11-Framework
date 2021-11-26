#include "GameScreen_Editor.h"

#include "../Models/Model.h"

// --------------------------------------------------------- //

GameScreen_Editor::GameScreen_Editor(ShaderHandler& shaderHandler, InputHandler& inputHandler) 
	: GameScreen(shaderHandler, inputHandler)
	, testCube(nullptr)
	, testCube2(nullptr)
	, mCamera(nullptr)
	, mGrid(shaderHandler)
	, mPostProcessing(nullptr)
	, mSkyDome(nullptr)
{
	testCube  = new TestCube(shaderHandler, Vector3D(3.0f, 0.0f, 0.0f));

	testCube2 = new TestCube(shaderHandler, Vector3D(3.0f, 0.0f, 3.0f));

	testCar   = new Car(shaderHandler);

	mGround   = new Ground(shaderHandler);

	mSkyDome  = new SkyDome(shaderHandler, Vector3D::zero, 200.0f, 50);

	mCamera   = new ThirdPersonCamera(&inputHandler,
		                               shaderHandler,
									Vector3D(0.0f, 0.0f, 0.0f),
									30.0f,
									Vector3D(1.0f, 0.0f, 0.0f),
									Vector3D(0.0f, 0.9f, 0.05f),
									DirectX::XMConvertToRadians(60.0f),
									0.01f,
									800.0f,
									16.0f / 9.0f,
									0.01f,
									0.1f);

	// Now setup the post processing stuff
	mPostProcessing = new PostProcessing(shaderHandler);

	mShaderHandler.CreateRasterizerState(&renderState);
}

// --------------------------------------------------------- //

GameScreen_Editor::~GameScreen_Editor()
{
	delete mCamera;
	mCamera = nullptr;

	delete testCube;
	testCube = nullptr;

	delete testCube2;
	testCube2 = nullptr;

	delete mPostProcessing;
	mPostProcessing = nullptr;

	delete testCar;
	testCar = nullptr;

	delete mSkyDome;
	mSkyDome = nullptr;

	delete mGround;
	mGround = nullptr;

	if (renderState)
	{
		renderState->Release();
		renderState = nullptr;
	}
}

// --------------------------------------------------------- //

void GameScreen_Editor::Render()
{
	if (!mPostProcessing)
		return;

	// Bind the post processing final image so we are rendering to that render target
	mPostProcessing->BindRenderTarget();

	// First clear the previous data from the post processing render target
	mPostProcessing->ClearRenderTarget();

	// ------------------------------------------------------------------------------------------- //

	mShaderHandler.BindRasterizerState(renderState);

	if(mGround)
		mGround->Render(mCamera);

	// Render the test cubes
	if (testCube)
		testCube->Render(mCamera);

	if (testCube2)
		testCube2->Render(mCamera);

	if (testCar)
		testCar->RenderFull(mCamera);

	// The grid for the track
	mGrid.Render(mCamera, mInputHandler);

	if (mCamera)
		mCamera->RenderFocusPoint();

	if (mSkyDome)
		mSkyDome->Render(mCamera);

	// ------------------------------------------------------------------------------------------- //

	mShaderHandler.BindRasterizerState(renderState);
	mPostProcessing->Render();

	// ------------------------------------------------------------------------------------------- //
}

// ------------------------------------------------------------------- //

void GameScreen_Editor::Update(const float deltaTime)
{
	if (mCamera)
		mCamera->Update(deltaTime);

	if (mInputHandler.GetIsMouseButtonPressed(4))
	{
		testCube->move(deltaTime);
	}

	if (mGround)
		mGround->Update(deltaTime);

	if (testCube)
		testCube->Update(deltaTime);

	if (testCube2)
	{
		testCube2->Update(deltaTime);
	}

	if (testCar)
		testCar->Update(deltaTime);

	if (mSkyDome)
		mSkyDome->Update(deltaTime);

	mGrid.Update(deltaTime);
}

// ------------------------------------------------------------------- //