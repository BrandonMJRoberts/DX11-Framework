#include "GameScreen_Editor.h"

#include "../Models/Model.h"
#include "../Sky/Sky.h"
#include "../Test/TestCube.h"
#include "../Camera/ThirdPersonCamera.h"
#include "../Models/Car.h"
#include "../Post Processing/PostProcessing.h"
#include "../Editor/RaceTrack.h"

// --------------------------------------------------------- //

GameScreen_Editor::GameScreen_Editor(ShaderHandler& shaderHandler, InputHandler& inputHandler) 
	: GameScreen(shaderHandler, inputHandler)
	, testCube(nullptr)
	, testCube2(nullptr)
	, mCamera(nullptr)
	, mPostProcessing(nullptr)
	, mSkyDome(nullptr)
    , mRaceTrack(nullptr)
{
	testCube  = new TestCube(shaderHandler, Vector3D(3.0f, 0.0f, 0.0f));

	testCube2 = new TestCube(shaderHandler, Vector3D(3.0f, 0.0f, 3.0f));

	testCar   = new Car(shaderHandler);

	mSkyDome  = new SkyDome(shaderHandler, Vector3D::zero, 200.0f, 50);

	mRaceTrack = new RaceTrack(shaderHandler, inputHandler, "filler");

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
									8.0f,
									90.0);

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

	delete mRaceTrack;
	mRaceTrack = nullptr;

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

	if(mRaceTrack)
		mRaceTrack->RenderGround(mCamera);

	// Render the test cubes
	if (testCube)
		testCube->Render(mCamera);

	if (testCube2)
		testCube2->Render(mCamera);

	if (testCar)
		testCar->RenderFull(mCamera);

	if (mCamera)
		mCamera->RenderFocusPoint();

	if (mRaceTrack)
		mRaceTrack->RenderGrid(mCamera);

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

	if (mRaceTrack)
		mRaceTrack->Update(deltaTime);

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
}

// ------------------------------------------------------------------- //