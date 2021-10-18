#ifndef _GAMESCREEN_EDITOR_H_
#define _GAMESCREEN_EDITOR_H_

#include "GameScreen.h"

class GameScreen_Editor final : public GameScreen
{
public:
	GameScreen_Editor(ShaderHandler& shaderHandler, InputHandler& inputHandler);
	~GameScreen_Editor();

private:
};

#endif