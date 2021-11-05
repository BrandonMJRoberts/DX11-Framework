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
{
	testCube = new TestCube(shaderHandler, Vector3D(3.0f, 0.0f, 0.0f));

	testCube2 = new TestCube(shaderHandler, Vector3D(3.0f, 0.0f, 3.0f));

	carTest = new Model(shaderHandler, "Models/Car/Car.obj");

	mCamera = new ThirdPersonCamera(&inputHandler,
									Vector3D(3.0f, 0.0f, 0.0f),
									10.0f,
									Vector3D(1.0f, 0.0f, 0.0f),
									Vector3D::worldUp,
									DirectX::XMConvertToRadians(60.0f),
									0.01f,
									800.0f,
									16.0f / 9.0f,
									0.1f,
									0.2f);

	// Now setup the post processing stuff
	mPostProcessing = new PostProcessing(shaderHandler);
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

	delete carTest;
	carTest = nullptr;
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

	// Render the test cubes
	if (testCube)
		testCube->Render(mCamera);

	if (testCube2)
		testCube2->Render(mCamera);

	// Now render the grid for the track
	//mGrid.Render(mCamera);

	// ------------------------------------------------------------------------------------------- //

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

	if (testCube)
		testCube->Update(deltaTime);

	if (testCube2)
	{
		testCube2->Update(deltaTime);
	}

	mGrid.Update(deltaTime);
}

// ------------------------------------------------------------------- //