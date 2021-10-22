#ifndef _GAMESCREEN_MAINMENU_H_
#define _GAMESCREEN_MAINMENU_H_

#include "GameScreen.h"

#include "../Test/TestCube.h"
#include "../Camera/ThirdPersonCamera.h"

class GameScreen_MainMenu final : public GameScreen
{
public:
	GameScreen_MainMenu(ShaderHandler& shaderHandler, InputHandler& inputHandler);
	~GameScreen_MainMenu();

	void Render() override;
	void Update(const float deltaTime) override;

private:
	ThirdPersonCamera* mCamera;

	TestCube* testCube;
	TestCube* testCube2;
};

#endif