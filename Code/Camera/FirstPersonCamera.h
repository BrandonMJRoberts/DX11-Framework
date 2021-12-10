#ifndef _FIRST_PERSON_CAMERA_H_
#define _FIRST_PERSON_CAMERA_H_

#include "BaseCamera.h"

class FirstPersonCamera : public BaseCamera
{
public:
	//FirstPersonCamera();
	FirstPersonCamera(InputHandler* inputHandler, Vector3D startPosition, Vector3D right, Vector3D up, float FOV, float nearPlane = 0.01f, float farPlane = 100.0f, float aspect = 16.0f / 9.0f, float movementSpeed = 15.0f);
	~FirstPersonCamera() override;

	void     Update(const float deltaTime) override;

	Vector3D GetFacingDirection()    override { return mFacingDirection; }

private:
	void  MovementCheck(bool& changed, const float deltaTime);
	void  MouseWheelCheck(bool& changed, const float deltaTime);
	void  RotationCheck(bool& changed, const float deltaTime);

	void  CapToRotationBounds(float& angleToRotateBy);

	Vector3D mFacingDirection;

	float mXRotationAngle;
	float mYRotationAngle;

	float mScrollSpeed;
};

#endif