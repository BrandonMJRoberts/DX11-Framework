#include "GameScreen_Editor.h"

#include "../Models/Model.h"
#include "../Sky/Sky.h"
#include "../Test/TestCube.h"

#include "../Camera/ThirdPersonCamera.h"
#include "../Camera/FirstPersonCamera.h"
#include "../Camera/StaticFirstPersonCamera.h"

#include "../Models/Car.h"
#include "../Post Processing/PostProcessing.h"
#include "../Editor/RaceTrack.h"

// --------------------------------------------------------- //

GameScreen_Editor::GameScreen_Editor(ShaderHandler& shaderHandler, InputHandler& inputHandler) 
	: GameScreen(shaderHandler, inputHandler)
	, testCube(nullptr)
	, testCube2(nullptr)
	, mCurrentCamera(nullptr)
	, mThirdPersonCamera(nullptr)
	, mFirstPersonCamera(nullptr)
	, mPostProcessing(nullptr)
	, mSkyDome(nullptr)
    , mRaceTrack(nullptr)
	, mCurrentCameraMode(CameraMode::THIRD_PERSON)
	, mFilledRenderState(nullptr)
	, mWireframeRenderState(nullptr)
	, mTopDownCamera(nullptr)
	, mSideOnCamera(nullptr)
	, mCurrentFillMode(RenderFillMode::FILLED)
{
	testCube  = new TestCube(shaderHandler, Vector3D(3.0f, 0.0f, 0.0f));

	testCube2 = new TestCube(shaderHandler, Vector3D(3.0f, 0.0f, 3.0f));

	testCar   = new Car(shaderHandler);

	mSkyDome  = new SkyDome(shaderHandler, Vector3D::zero, 200.0f, 50);

	mRaceTrack = new RaceTrack(shaderHandler, inputHandler, "filler");

	mThirdPersonCamera = new ThirdPersonCamera(&inputHandler,
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

	mFirstPersonCamera = new FirstPersonCamera(&inputHandler, 
		                                       Vector3D(0.0f, 10.0f, -30.0f), 
		                                       Vector3D(1.0f, 0.0f, 0.0f), 
		                                       Vector3D(0.0f, 0.9f, 0.05f), 
		                                       DirectX::XMConvertToRadians(60.0f), 
		                                       0.01f, 
		                                       800.0f, 
		                                       16.0f / 9.0f,
		                                       20.0f);

	mSideOnCamera  = new StaticFirstPersonCamera(&inputHandler,
		                                   Vector3D(0.0f, 5.0f, -30.0f),
		                                   Vector3D(1.0f, 0.0f,  0.0f),
		                                   Vector3D(0.0f, 1.0f,  0.0f),
		                                   DirectX::XMConvertToRadians(60.0f),
		                                   10.0f,
		                                   10.0f,
		                                   0.01f,
		                                   800.0f,
		                                   16.0f / 9.0f,
		                                   20.0f);

	mTopDownCamera = new StaticFirstPersonCamera(&inputHandler,
		                                   Vector3D(0.0f, 60.0f, 0.0f),
		                                   Vector3D(1.0f, 0.0f, 0.0f),
		                                   Vector3D(0.0f, 0.0f, 1.0f),
		                                   DirectX::XMConvertToRadians(60.0f),
		                                   10.0f,
		                                   10.0f,
		                                   0.01f,
		                                   800.0f,
		                                   16.0f / 9.0f,
		                                   20.0f);

	// Now setup the post processing stuff
	mPostProcessing = new PostProcessing(shaderHandler);

	mShaderHandler.CreateRasterizerState(&mFilledRenderState);
	mShaderHandler.CreateRasterizerState(&mWireframeRenderState, D3D11_FILL_WIREFRAME, D3D11_CULL_FRONT);


	mCurrentCamera     = mThirdPersonCamera;
	mCurrentCameraMode = CameraMode::THIRD_PERSON;
}

// --------------------------------------------------------- //

GameScreen_Editor::~GameScreen_Editor()
{
	delete mThirdPersonCamera;
	mThirdPersonCamera = nullptr;

	delete mFirstPersonCamera;
	mFirstPersonCamera = nullptr;

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

	if (mFilledRenderState)
	{
		mFilledRenderState->Release();
		mFilledRenderState = nullptr;
	}

	if (mWireframeRenderState)
	{
		mWireframeRenderState->Release();
		mWireframeRenderState = nullptr;
	}

	mCurrentCamera = nullptr;
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

	if (mCurrentFillMode == RenderFillMode::FILLED)
	{
		mShaderHandler.BindRasterizerState(mFilledRenderState);
	}
	else if (mCurrentFillMode == RenderFillMode::WIREFRAME)
	{
		mShaderHandler.BindRasterizerState(mWireframeRenderState);
	}


	if(mRaceTrack)
		mRaceTrack->RenderGround(mCurrentCamera);

	// Render the test cubes
	if (testCube)
		testCube->Render(mCurrentCamera);

	if (testCube2)
		testCube2->Render(mCurrentCamera);

	if (testCar)
		testCar->RenderFull(mCurrentCamera);

	if (   mCurrentCameraMode == CameraMode::THIRD_PERSON
		&& mThirdPersonCamera)
	{
		mThirdPersonCamera->RenderFocusPoint();
	}

	if (mRaceTrack)
		mRaceTrack->RenderGrid(mCurrentCamera);

	if (mSkyDome)
		mSkyDome->Render(mCurrentCamera);

	// ------------------------------------------------------------------------------------------- //

	// Bind back to fill state for the final render, as it would just draw 2 triangles otherwise
	mShaderHandler.BindRasterizerState(mFilledRenderState);
	mPostProcessing->Render();

	// ------------------------------------------------------------------------------------------- //
}

// ------------------------------------------------------------------- //

void GameScreen_Editor::Update(const float deltaTime)
{
	CameraSwappingCheck();

	RenderFillStateSwapCheck();

	if (mCurrentCamera)
		mCurrentCamera->Update(deltaTime);

	if (mRaceTrack)
		mRaceTrack->Update(deltaTime, mCurrentCamera);

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

void GameScreen_Editor::CameraSwappingCheck()
{
	if (mInputHandler.GetIsKeyPressed('1'))
	{
		// Swap to third person camera
		if (mCurrentCameraMode == CameraMode::THIRD_PERSON || !mThirdPersonCamera)
			return;

		mCurrentCameraMode = CameraMode::THIRD_PERSON;
		mCurrentCamera     = mThirdPersonCamera;
	}
	else if (mInputHandler.GetIsKeyPressed('2'))
	{
		// Swap to first person camera
		if (mCurrentCameraMode == CameraMode::FIRST_PERSON || !mFirstPersonCamera)
			return;

		mCurrentCameraMode = CameraMode::FIRST_PERSON;
		mCurrentCamera     = mFirstPersonCamera;
	}
	else if (mInputHandler.GetIsKeyPressed('3'))
	{
		// Top down camera
		if (mCurrentCameraMode == CameraMode::TOP_DOWN || !mTopDownCamera)
			return;

		mCurrentCameraMode = CameraMode::TOP_DOWN;
		mCurrentCamera     = mTopDownCamera;
	}
	else if (mInputHandler.GetIsKeyPressed('4'))
	{
		// Side on camera that rotates around the edge of the play area
		if (mCurrentCameraMode == CameraMode::SIDE_ON || !mSideOnCamera)
			return;

		mCurrentCameraMode = CameraMode::SIDE_ON;
		mCurrentCamera     = mSideOnCamera;
	}
}

// ------------------------------------------------------------------- //

void GameScreen_Editor::RenderFillStateSwapCheck()
{
	if (mInputHandler.GetIsKeyPressed('9'))
	{
		// Go to normal rendering state
		if(mFilledRenderState)
			mCurrentFillMode = RenderFillMode::FILLED;
	}
	else if (mInputHandler.GetIsKeyPressed('0'))
	{
		// Go to wireframe mode
		if(mWireframeRenderState)
			mCurrentFillMode = RenderFillMode::WIREFRAME;
	}
}

// ------------------------------------------------------------------- //