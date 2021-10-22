#ifndef _THIRD_PERSON_CAMERA_H_
#define _THIRD_PERSON_CAMERA_H_

#include "BaseCamera.h"

class ThirdPersonCamera : public BaseCamera
{
public:
	ThirdPersonCamera();
	ThirdPersonCamera(InputHandler* inputHandler, 
		              Vector3D      focalPoint, 
		              float         distanceFromFocalPoint, 
		              Vector3D      right, 
		              Vector3D      up,  
		              float         FOV, 
		              float         nearPlane     = 0.01f, 
		              float         farPlane      = 100.0f, 
		              float         aspect        = 16.0f / 9.0f, 
		              float         movementSpeed = 1.0f,
		              float         rotationSpeed = 0.1f);
	~ThirdPersonCamera() override;

	void Update(const float deltaTime) override;

	void ReCalculateViewMatrix() override;

private:
	void ReCalculatePosition();

	const float kMaxDistance = 15.0f;
	const float kMinDistance = 2.0f;

	Vector3D mFocalPoint;
	float    mDistanceFromFocalPoint;
};

#endif