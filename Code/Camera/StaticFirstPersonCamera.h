#ifndef _STATIC_FIRST_PERSON_CAMERA_H_
#define _STATIC_FIRST_PERSON_CAMERA_H_

#include "FirstPersonCamera.h"

class StaticFirstPersonCamera : public FirstPersonCamera
{
public:
	StaticFirstPersonCamera(InputHandler* inputHandler, 
		                    Vector3D      startPosition, 
		                    Vector3D      right, 
		                    Vector3D      up, 
		                    float         FOV, 
							float         forwardMovementCap   = 10.0f,
							float         backwardsMovementCap = 10.0f,
		                    float         nearPlane     = 0.01f, 
		                    float         farPlane      = 100.0f, 
		                    float         aspect        = 16.0f / 9.0f, 
		                    float         movementSpeed = 15.0f);
	~StaticFirstPersonCamera() override;

	void Update(const float deltaTime) override;

private:
	void  MovementCheck(bool& changed, const float deltaTime)   override;
	void  MouseWheelCheck(bool& changed, const float deltaTime) override;

	float mForwardMovementCap;
	float mBackwardsMovementCap;

	float mScrollMovementApplied;
};

#endif