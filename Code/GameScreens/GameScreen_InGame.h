#ifndef _GAMESCREEN_INGAME_H_
#define _GAMESCREEN_INGAME_H_

#include "GameScreen.h"

class GameScreen_InGame final : public GameScreen
{
public:
	GameScreen_InGame(ShaderHandler& shaderHandler, InputHandler& inputHandler);
	~GameScreen_InGame();

private:
};

#endif