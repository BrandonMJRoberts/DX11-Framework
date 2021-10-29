#include "GameScreen_Editor.h"

// --------------------------------------------------------- //

GameScreen_Editor::GameScreen_Editor(ShaderHandler& shaderHandler, InputHandler& inputHandler) 
	: GameScreen(shaderHandler, inputHandler)
	, testCube(nullptr)
	, testCube2(nullptr)
	, mCamera(nullptr)
	, mGrid(shaderHandler)
{
	testCube = new TestCube(shaderHandler, Vector3D(3.0f, 0.0f, 0.0f));

	testCube2 = new TestCube(shaderHandler, Vector3D(3.0f, 0.0f, 3.0f));

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
}

// --------------------------------------------------------- //


void GameScreen_Editor::Render()
{
	if (testCube)
		testCube->Render(mCamera);

	if (testCube2)
		testCube2->Render(mCamera);

	mGrid.Render(mCamera);
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