#ifndef _GAMESCREEN_H_
#define _GAMESCREEN_H_

#include "../Shaders/ShaderHandler.h"

class GameScreen
{
public:
	GameScreen(ShaderHandler& shaderHandler);
	~GameScreen();

	virtual void Render();
	virtual void Update(const float deltaTime);

private:
	ShaderHandler& mShaderHandler;
};

#endif