#include "FirstPersonCamera.h"

#include "../GameScreens/ScreenManager.h"

// ----------------------------------------------------------------- //

FirstPersonCamera::FirstPersonCamera(InputHandler* inputHandler, Vector3D startPosition, Vector3D right, Vector3D up, float FOV, float nearPlane, float farPlane, float aspect, float movementSpeed, float rotationSpeed)
	: BaseCamera(inputHandler, startPosition, right, up, FOV, nearPlane, farPlane, aspect, movementSpeed, rotationSpeed)
	, mScrollSpeed(15.0f)
	, mXRotationAngle(0.0f)
	, mYRotationAngle(0.0f)
	, mCameraAngleToVertical(PI)
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
	else if (mInputHandler->GetIsKeyPressed(0)) // Shift down key ID
	{
		mPosition -= mUp * mMovementSpeed * deltaTime;
		changed    = true;
	}
}

// ----------------------------------------------------------------- //

void FirstPersonCamera::RotationCheck(bool& changed, const float deltaTime)
{
	if (!mInputHandler->GetIsMouseButtonPressed(1))
		return;

	// Check to see if the user has moved the mouse to look around
	Vector2D movementDistance = mInputHandler->GetMouseMovementDelta();

	// Quick out
	if (movementDistance.x == 0.0f && movementDistance.y == 0.0f)
		return;

	changed = true;

	// Get the fraction of the screen the player has moved the mouse to determine a rotation amount
	Vector2D fraction = Vector2D(movementDistance.x / GameScreenManager::ScreenWidth, movementDistance.y / GameScreenManager::ScreenHeight);
	fraction.x *= TWOPI;
	fraction.y *= PI;

	// Now rotate around the camera's right
	RotateAroundCameraRight(fraction.y * mRotationSpeed);

	// Now rotate around the world up
	RotateAroundWorldUp(fraction.x * mRotationSpeed);
}

// ----------------------------------------------------------------- //

void FirstPersonCamera::RotateAroundCameraRight(float angle)
{
	angle = -angle;
	const float max = (3 * PI) / 2.0f, min = PIDIV2;

	// Make sure we are capping the rotation correctly
	if (mCameraAngleToVertical + angle > max)
	{
		angle = max - mCameraAngleToVertical;
	}
	else if (mCameraAngleToVertical + angle < min)
	{
		angle = min - mCameraAngleToVertical;
	}

	mCameraAngleToVertical += angle;

	// Get the rotation matrix
	DirectX::XMFLOAT3X3 rotationMatrix = MatrixMaths::AxisRotationMatrix(mRight, angle);
	
	// Apply the rotations	
	mFacingDirection = mFacingDirection * rotationMatrix;
	mUp              = mUp * rotationMatrix;

	mFacingDirection.Normalise();
	mUp.Normalise();
}

// ----------------------------------------------------------------- //

void FirstPersonCamera::RotateAroundWorldUp(float angle)
{
	// Get the rotation matrix
	DirectX::XMFLOAT3X3 rotationMatrix = MatrixMaths::AxisRotationMatrix(Vector3D(0.0f, 1.0f, 0.0f), -angle);

	// Now apply the rotation to the facing direction
	mFacingDirection = mFacingDirection * rotationMatrix;

	// Apply the rotation to the camera's right
	mRight = mRight * rotationMatrix;

	mUp = mUp * rotationMatrix;

	mRight.Normalise();
	mFacingDirection.Normalise();
	mUp.Normalise();
}

// ----------------------------------------------------------------- //

void FirstPersonCamera::MouseWheelCheck(bool& changed, const float deltaTime)
{
	UNREFERENCED_PARAMETER(changed);
	UNREFERENCED_PARAMETER(deltaTime);
}

// ----------------------------------------------------------------- //