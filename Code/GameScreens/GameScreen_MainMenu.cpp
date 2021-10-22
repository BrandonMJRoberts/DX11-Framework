#include "GameScreen_MainMenu.h"

// ------------------------------------------------------------------- //

GameScreen_MainMenu::GameScreen_MainMenu(ShaderHandler& shaderHandler, InputHandler& inputHandler) 
	: GameScreen(shaderHandler, inputHandler)
	, testCube(nullptr)
	, testCube2(nullptr)
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
		                            100.0f, 
		                            16.0f / 9.0f, 
		                            0.1f,  
		                            DirectX::XMConvertToRadians(10.0f));
}

// ------------------------------------------------------------------- //

GameScreen_MainMenu::~GameScreen_MainMenu()
{
	delete mCamera;
	mCamera = nullptr;

	delete testCube;
	testCube = nullptr;

	delete testCube2;
	testCube2 = nullptr;
}

// ------------------------------------------------------------------- //

void GameScreen_MainMenu::Render()
{
	if (testCube)
		testCube->Render(mCamera);

	if (testCube2)
		testCube2->Render(mCamera);
}

// ------------------------------------------------------------------- //

void GameScreen_MainMenu::Update(const float deltaTime)
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
}

// ------------------------------------------------------------------- //