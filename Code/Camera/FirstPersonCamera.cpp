#include "FirstPersonCamera.h"

// ----------------------------------------------------------------- //

//FirstPersonCamera::FirstPersonCamera() 
//	: BaseCamera()
//{
//
//}

// ----------------------------------------------------------------- //

FirstPersonCamera::FirstPersonCamera(InputHandler* inputHandler, Vector3D startPosition, Vector3D right, Vector3D up, float FOV, float nearPlane, float farPlane, float aspect, float movementSpeed)
	: BaseCamera(inputHandler, startPosition, right, up, FOV, nearPlane, farPlane, aspect, movementSpeed, 15.0f)
	, mScrollSpeed(15.0f)
	, mXRotationAngle(0.0f)
	, mYRotationAngle(0.0f)
{
	ReCalculatePerspectiveMatrix();
	ReCalculateViewMatrix();

	mFacingDirection = mRight.Cross(mUp);
}

// ----------------------------------------------------------------- //

FirstPersonCamera::~FirstPersonCamera()
{

}

// ----------------------------------------------------------------- //

void FirstPersonCamera::Update(const float deltaTime)
{
	bool changed = false;

	MovementCheck(changed, deltaTime);

	RotationCheck(changed, deltaTime);

	if (changed)
		ReCalculateViewMatrix();
}

// ----------------------------------------------------------------- //

void FirstPersonCamera::MovementCheck(bool& changed, const float deltaTime)
{
	if(mInputHandler->GetIsKeyPressed('W'))
	{
		// Forward movement
		mPosition += mFacingDirection * deltaTime * mMovementSpeed;
		changed    = true;
	}
	else if (mInputHandler->GetIsKeyPressed('S'))
	{
		// Backward movement
		mPosition -= mFacingDirection * deltaTime * mMovementSpeed;
		changed    = true;
	}

	if (mInputHandler->GetIsKeyPressed('A'))
	{
		// Left movement
		mPosition -= mRight * mMovementSpeed * deltaTime;
		changed    = true;
	}
	else if (mInputHandler->GetIsKeyPressed('D'))
	{
		// Right movement
		mPosition += mRight * mMovementSpeed * deltaTime;
		changed    = true;
	}

	if (mInputHandler->GetIsKeyPressed(32)) // Space press check
	{
		mPosition += mUp * mMovementSpeed * deltaTime;
		changed    = true;
	}
	//else if (mInputHandler->GetIsKeyPressed()) // Shift down key ID
	//{
	//	mPosition -= mUp * mMovementSpeed * deltaTime;
	//	changed    = true;
	//}
}

// ----------------------------------------------------------------- //

void FirstPersonCamera::RotationCheck(bool& changed, const float deltaTime)
{
	// Check to see if the user has moved the mouse to look around
	Vector2D movementDistance = mInputHandler->GetMouseMovementDelta();

	if (movementDistance.x == 0.0f && movementDistance.y == 0.0f)
		return;


	changed = true;

	float angle = 0.0f * deltaTime;
	CapToRotationBounds(angle);

	// Re-calculate the facing direction
	//mFacingDirection = ;
}

// ----------------------------------------------------------------- //

void FirstPersonCamera::CapToRotationBounds(float& angleToRotateBy)
{
	UNREFERENCED_PARAMETER(angleToRotateBy);
}

// ----------------------------------------------------------------- //

void FirstPersonCamera::MouseWheelCheck(bool& changed, const float deltaTime)
{
	UNREFERENCED_PARAMETER(changed);
	UNREFERENCED_PARAMETER(deltaTime);
}

// ----------------------------------------------------------------- //