#ifndef _GAMESCREEN_EDITOR_H_
#define _GAMESCREEN_EDITOR_H_

#include "GameScreen.h"

class RaceTrack;
class Model;
class PostProcessing;

class BaseCamera;
class ThirdPersonCamera;
class FirstPersonCamera;
class StaticFirstPersonCamera;

class Car;
class TestCube;
class SkyDome;

// -----------------------------------------------------------------------------------------

enum class CameraMode
{
	FIRST_PERSON,
	THIRD_PERSON,

	TOP_DOWN,
	SIDE_ON,

	MAX
};

// -----------------------------------------------------------------------------------------

class GameScreen_Editor final : public GameScreen
{
public:
	GameScreen_Editor(ShaderHandler& shaderHandler, InputHandler& inputHandler);
	~GameScreen_Editor();

	void Render() override;
	void Update(const float deltaTime) override;

private:
	void CameraSwappingCheck();

	PostProcessing*        mPostProcessing;

	// Cameras
	BaseCamera*            mCurrentCamera;

	ThirdPersonCamera*           mThirdPersonCamera;
	FirstPersonCamera*           mFirstPersonCamera;
	StaticFirstPersonCamera*     mSideOnCamera;
	StaticFirstPersonCamera*     mTopDownCamera;

	TestCube*              testCube;
	TestCube*              testCube2;
	Car*                   testCar;
	SkyDome*               mSkyDome;
	RaceTrack*             mRaceTrack;
	ID3D11RasterizerState* mRenderState;

	CameraMode             mCurrentCameraMode;
};

// -----------------------------------------------------------------------------------------

#endif