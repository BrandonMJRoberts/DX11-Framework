#ifndef _GAMESCREEN_H_
#define _GAMESCREEN_H_

#include "../Shaders/ShaderHandler.h"
#include "../Input/InputHandler.h"

class GameScreen
{
public:
	GameScreen(ShaderHandler& shaderHandler, InputHandler& inputHandler);
	~GameScreen();

	virtual void Render();
	virtual void Update(const float deltaTime);

protected:
	ShaderHandler& mShaderHandler;
	InputHandler&  mInputHandler;
};

#endif