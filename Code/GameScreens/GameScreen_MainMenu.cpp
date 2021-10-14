#include "GameScreen_MainMenu.h"

// ------------------------------------------------------------------- //

GameScreen_MainMenu::GameScreen_MainMenu(ShaderHandler& shaderHandler) : GameScreen(shaderHandler)
{
	testCube = new TestCube(shaderHandler);
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
}

// ------------------------------------------------------------------- //

void GameScreen_MainMenu::Update(const float deltaTime)
{
	if (testCube)
		testCube->Update(deltaTime);
}

// ------------------------------------------------------------------- //