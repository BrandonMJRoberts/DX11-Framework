#ifndef _GAMESCREEN_EDITOR_H_
#define _GAMESCREEN_EDITOR_H_

#include "GameScreen.h"

#include "../Test/TestCube.h"
#include "../Camera/ThirdPersonCamera.h"

#include "../Editor/Grid/Grid.h"

#include "../Post Processing/PostProcessing.h"

class GameScreen_Editor final : public GameScreen
{
public:
	GameScreen_Editor(ShaderHandler& shaderHandler, InputHandler& inputHandler);
	~GameScreen_Editor();

	void Render() override;
	void Update(const float deltaTime) override;

private:
	PostProcessing* mPostProcessing;

	// Internal grid for the track
	EditorGrid         mGrid;

	ThirdPersonCamera* mCamera;

	TestCube* testCube;
	TestCube* testCube2;
};

#endif