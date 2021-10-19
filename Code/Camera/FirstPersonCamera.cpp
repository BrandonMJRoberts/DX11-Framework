#include "FirstPersonCamera.h"

// ----------------------------------------------------------------- //

FirstPersonCamera::FirstPersonCamera() 
	: BaseCamera()
{

}

// ----------------------------------------------------------------- //

FirstPersonCamera::FirstPersonCamera(InputHandler* inputHandler, Vector3D startPosition, Vector3D right, Vector3D up, float FOV, float nearPlane, float farPlane, float aspect)
	: BaseCamera(inputHandler, startPosition, right, up, FOV, nearPlane, farPlane, aspect)
{

}

// ----------------------------------------------------------------- //

FirstPersonCamera::~FirstPersonCamera()
{

}

// ----------------------------------------------------------------- //

void FirstPersonCamera::Update(const float deltaTime)
{

}

// ----------------------------------------------------------------- //