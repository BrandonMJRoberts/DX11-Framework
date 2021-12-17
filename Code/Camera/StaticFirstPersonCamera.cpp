#include "StaticFirstPersonCamera.h"

// ---------------------------------------------------------------------

StaticFirstPersonCamera::StaticFirstPersonCamera(InputHandler* inputHandler, Vector3D startPosition, Vector3D right, Vector3D up, float FOV, float forwardMovementCap, float backwardsMovementCap, float nearPlane, float farPlane, float aspect, float movementSpeed)
	: FirstPersonCamera(inputHandler, startPosition, right, up, FOV, nearPlane, farPlane, aspect, movementSpeed),
	mForwardMovementCap(forwardMovementCap),
	mBackwardsMovementCap(backwardsMovementCap),
	mScrollMovementApplied(0.0f)
{

}

// ---------------------------------------------------------------------

StaticFirstPersonCamera::~StaticFirstPersonCamera()
{

}

// ---------------------------------------------------------------------

void StaticFirstPersonCamera::Update(const float deltaTime)
{
	bool changed = false;

	MovementCheck(changed, deltaTime);

	MouseWheelCheck(changed, deltaTime);

	if (changed)
		ReCalculateViewMatrix();
}

// ---------------------------------------------------------------------

void  StaticFirstPersonCamera::MovementCheck(bool& changed, const float deltaTime)
{
	if (mInputHandler->GetIsKeyPressed('W'))
	{
		// Forward movement
		mPosition += mUp * deltaTime * mMovementSpeed;
		changed = true;
	}
	else if (mInputHandler->GetIsKeyPressed('S'))
	{
		// Backward movement
		mPosition -= mUp * deltaTime * mMovementSpeed;
		changed = true;
	}

	if (mInputHandler->GetIsKeyPressed('A'))
	{
		// Left movement
		mPosition -= mRight * mMovementSpeed * deltaTime;
		changed = true;
	}
	else if (mInputHandler->GetIsKeyPressed('D'))
	{
		// Right movement
		mPosition += mRight * mMovementSpeed * deltaTime;
		changed = true;
	}

	// Now bind the camera to the play area
	if (mPosition.y < 1.0f)
		mPosition.y = 1.0f;
}

// ---------------------------------------------------------------------

void  StaticFirstPersonCamera::MouseWheelCheck(bool& changed, const float deltaTime)
{
	// With these cameras using the scroll wheel will move the camera in/out slightly
	char scrollData = mInputHandler->GetScrollWheelFrameDelta();

	if (scrollData == 0)
	{
		return;
	}

	Vector3D movementVector   =  mFacingDirection         * mScrollSpeed;
	float   movementPotential = (movementVector).Length() * deltaTime    * scrollData;

	if (mScrollMovementApplied + movementPotential > mForwardMovementCap)
	{
		movementPotential      = mForwardMovementCap - mScrollMovementApplied;
		mScrollMovementApplied = mForwardMovementCap;
	}
	else if (mScrollMovementApplied + movementPotential < -mBackwardsMovementCap)
	{
		movementPotential      = -(mBackwardsMovementCap + mScrollMovementApplied);
		mScrollMovementApplied = -mBackwardsMovementCap;
	}
	else
	{
		mScrollMovementApplied += movementPotential;
	}

	mPosition += movementVector.Normalised() * movementPotential;
	changed    = true;
}

// ---------------------------------------------------------------------