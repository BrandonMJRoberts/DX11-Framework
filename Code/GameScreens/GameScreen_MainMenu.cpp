#include "GameScreen_MainMenu.h"

// ------------------------------------------------------------------- //

GameScreen_MainMenu::GameScreen_MainMenu(ShaderHandler& shaderHandler, InputHandler& inputHandler) 
	: GameScreen(shaderHandler, inputHandler)
	, testCube(nullptr)
	, testCube2(nullptr)
{
	testCube = new TestCube(shaderHandler, Vector3D(3.0f, 0.0f, 0.0f));

	testCube2 = new TestCube(shaderHandler, Vector3D(3.0f, 0.0f, 3.0f));
}

// ------------------------------------------------------------------- //

GameScreen_MainMenu::~GameScreen_MainMenu()
{

}

// ------------------------------------------------------------------- //

void GameScreen_MainMenu::Render()
{
	if (testCube)
		testCube->Render();

	if (testCube2)
		testCube2->Render();
}

// ------------------------------------------------------------------- //

void GameScreen_MainMenu::Update(const float deltaTime)
{
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