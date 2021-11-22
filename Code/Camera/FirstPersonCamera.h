#ifndef _FIRST_PERSON_CAMERA_H_
#define _FIRST_PERSON_CAMERA_H_

#include "BaseCamera.h"

class FirstPersonCamera : public BaseCamera
{
public:
	FirstPersonCamera();
	FirstPersonCamera(InputHandler* inputHandler, Vector3D startPosition, Vector3D right, Vector3D up, float FOV, float nearPlane = 0.01f, float farPlane = 100.0f, float aspect = 16.0f / 9.0f);
	~FirstPersonCamera() override;

	void Update(const float deltaTime) override;

	Vector3D GetFacingDirection() override { return Vector3D(); }
private:
};

#endif