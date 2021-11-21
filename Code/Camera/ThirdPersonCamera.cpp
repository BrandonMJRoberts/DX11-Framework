#include "ThirdPersonCamera.h"

#include "../GameScreens/ScreenManager.h"

// ------------------------------------------------------------------------- //

ThirdPersonCamera::ThirdPersonCamera()
	: BaseCamera()
	, mFocalPoint(0.0f, 0.0f, 0.0f)
	, mDistanceFromFocalPoint(10.0f)
	, mYRotationAngle(0.0f)
	, mXRotationAngle(0.0f)
{
	float zero = 0.0f;
	CapToYRotationBounds(zero);
	CapToXRotationBounds(zero);

	ReCalculatePosition();

	ReCalculatePerspectiveMatrix();
	ReCalculateViewMatrix();
}

// ------------------------------------------------------------------------- //

ThirdPersonCamera::ThirdPersonCamera(InputHandler* inputHandler, 
	                                 Vector3D      focalPoint, 
	                                 float         distanceFromFocalPoint, 
	                                 Vector3D      right, 
	                                 Vector3D      up, 
	                                 float         FOV, 
	                                 float         nearPlane, 
	                                 float         farPlane, 
	                                 float         aspect,
	                                 float         movementSpeed, 
	                                 float         rotationSpeed)
	: BaseCamera(inputHandler, Vector3D::zero, right.Normalised(), up.Normalised(), FOV, nearPlane, farPlane, aspect, movementSpeed, rotationSpeed)
	, mFocalPoint(focalPoint)
	, mDistanceFromFocalPoint(distanceFromFocalPoint)
	, mYRotationAngle(0.0f)
	, mXRotationAngle(0.0f)
{
	float zero = 0.0f;
	CapToYRotationBounds(zero);
	CapToXRotationBounds(zero);

	ReCalculatePosition();

	ReCalculatePerspectiveMatrix();
	ReCalculateViewMatrix();
}

// ------------------------------------------------------------------------- //

ThirdPersonCamera::~ThirdPersonCamera()
{
	
}

// ------------------------------------------------------------------------- //

void ThirdPersonCamera::Update(const float deltaTime) 
{
	// Check to see if the player is trying to move the camera
	if (mInputHandler)
	{
		bool changed = false;

		// Check to see if the player has moved the focal point
		MovementCheck(changed, deltaTime);

		// Check for the mouse wheel scrolling
		MouseWheelCheck(changed, deltaTime);

		// Now check to see if the player is rotating the camera around the focal point
		RotationalCheck(changed, deltaTime);

		if (changed)
		{
			ReCalculateViewMatrix();
		}

		// ---------------------------------------------------------------------------------------------------
	}
}

// ------------------------------------------------------------------------- //

void ThirdPersonCamera::ReCalculateViewMatrix()
{
	// Calculate view matrix
	DirectX::XMStoreFloat4x4(&mViewMatrix, DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(mPosition.x,   mPosition.y,   mPosition.z,   0.0f), 
																	 DirectX::XMVectorSet(mFocalPoint.x, mFocalPoint.y, mFocalPoint.z, 0.0f),
																	 DirectX::XMVectorSet(mUp.x,         mUp.y,         mUp.z,         0.0f)));
}

// ------------------------------------------------------------ //

void ThirdPersonCamera::ReCalculatePosition()
{
	// Calculate the facing direction
	Vector3D facingDir = mRight.Cross(mUp);

	// Now calculate the position
	mPosition = mFocalPoint - (facingDir.Normalised() * mDistanceFromFocalPoint);
}

// ------------------------------------------------------------ //

void ThirdPersonCamera::MouseWheelCheck(bool& changed, const float deltaTime)
{
	// Now check to see if the player has scrolled the mouse wheel in/out
	int wheelDelta = mInputHandler->GetScrollWheelFrameDelta();
	if (wheelDelta != 0)
	{
		changed = true;

		// Change the distance 
		if (wheelDelta > 0)
		{
			mDistanceFromFocalPoint += deltaTime * mMovementSpeed;

			if (mDistanceFromFocalPoint > kMaxDistance)
				mDistanceFromFocalPoint = kMaxDistance;
		}
		else
		{
			mDistanceFromFocalPoint -= deltaTime * mMovementSpeed;

			if (mDistanceFromFocalPoint < kMinDistance)
				mDistanceFromFocalPoint = kMinDistance;
		}
	}
}

// ------------------------------------------------------------ //

void ThirdPersonCamera::MovementCheck(bool& changed, const float deltaTime)
{
	// ---------------------------------------------------------------------------------------------------

	// Left/right
	if (mInputHandler->GetIsKeyPressed('A'))
	{
		changed = true;
		mFocalPoint += (mRight * -mMovementSpeed) * deltaTime;
	}
	else if (mInputHandler->GetIsKeyPressed('D'))
	{
		changed = true;
		mFocalPoint += (mRight * mMovementSpeed) * deltaTime;
	}

	// Up/down - only one layer so disabled
	/*if (mInputHandler->GetIsKeyPressed(VK_SPACE))
	{
		changed = true;
		mFocalPoint += (Vector3D::worldUp * mMovementSpeed) * deltaTime;
	}
	else if (mInputHandler->GetIsKeyPressed('X'))
	{
		changed = true;
		mFocalPoint += (Vector3D::worldUp * -mMovementSpeed) * deltaTime;
	}*/


	// As we need to restrict the movement to the X/Z plane we need to adjust the facing direction
	Vector3D facingDirection = mRight.Cross(mUp);
	facingDirection.y = 0.0f;
	facingDirection.Normalise();

	// Now for forward/backward
	if (mInputHandler->GetIsKeyPressed('W'))
	{
		changed = true;
		mFocalPoint += (facingDirection * mMovementSpeed) * deltaTime;
	}
	else if (mInputHandler->GetIsKeyPressed('S'))
	{
		changed = true;
		mFocalPoint += (facingDirection * -mMovementSpeed) * deltaTime;
	}

	// Now re-calculate the position of the camera after the changes have been made
	ReCalculatePosition();
}

// ------------------------------------------------------------ //

void ThirdPersonCamera::RotationalCheck(bool& changed, const float deltaTime)
{
	// Check for the right mouse button press
	if (mInputHandler->GetIsMouseButtonPressed(1))
	{
		// Now check to see if the player is moving the mouse
		Vector2D mouseDelta = mInputHandler->GetMouseMovementDelta();

		// If the mouse is moved at all
		if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)
		{
			// Set that the view matrix has changed
			changed = true;

			// Get the offset vector - from the focal point to the position
			Vector3D offsetVector = mPosition - mFocalPoint;

			Vector2D fractions = Vector2D(mouseDelta.x / GameScreenManager::ScreenWidth, mouseDelta.y / GameScreenManager::ScreenHeight);
			fractions.x *= TWOPI;
			fractions.y *= PI;

			// Now calculate the rotation amount
			float angle;

			// Now rotate around the camera's right vector
			if (mouseDelta.y > 0.0f)
			{
				angle = fractions.y * mRotationSpeed * deltaTime;

				CapToYRotationBounds(angle);

				offsetVector *= MatrixMaths::AxisRotationMatrix(mRight, angle);
				mRight       *= MatrixMaths::AxisRotationMatrix(mRight, angle);
				mUp          *= MatrixMaths::AxisRotationMatrix(mRight, angle);
			}
			else if (mouseDelta.y < 0.0f)
			{
				angle = fractions.y * mRotationSpeed * deltaTime;

				CapToYRotationBounds(angle);

				offsetVector *= MatrixMaths::AxisRotationMatrix(mRight, angle);
				mRight       *= MatrixMaths::AxisRotationMatrix(mRight, angle);
				mUp          *= MatrixMaths::AxisRotationMatrix(mRight, angle);
			}
			
			// Now rotate that offset around the world up if we need to
			if (mouseDelta.x > 0.0f)
			{
				angle = fractions.x * mRotationSpeed * deltaTime;

				//CapToXRotationBounds(angle);

				offsetVector *= MatrixMaths::GetYAxisRotationMatrix(angle);
				mRight       *= MatrixMaths::GetYAxisRotationMatrix(angle);
				mUp          *= MatrixMaths::GetYAxisRotationMatrix(angle);
			}
			else if (mouseDelta.x < 0.0f)
			{
				angle = fractions.x * mRotationSpeed * deltaTime;

				//CapToXRotationBounds(angle);

				offsetVector *= MatrixMaths::GetYAxisRotationMatrix(angle);
				mRight       *= MatrixMaths::GetYAxisRotationMatrix(angle);
				mUp          *= MatrixMaths::GetYAxisRotationMatrix(angle);
			}

			mRight.Normalise();
			mUp.Normalise();

			mPosition = mFocalPoint + offsetVector;
		}
	}
}

// ------------------------------------------------------------ //

void ThirdPersonCamera::CapToXRotationBounds(float& angleToRotateBy)
{
	if (mXRotationAngle + angleToRotateBy > PIDIV2)
	{
		angleToRotateBy = PIDIV2 - mXRotationAngle;
		mXRotationAngle = PIDIV2;
	}
	else if (mXRotationAngle + angleToRotateBy < -PIDIV2)
	{
		angleToRotateBy = -(fabsf(PIDIV2 - fabsf(mXRotationAngle)));
		mXRotationAngle = -PIDIV2;
	}
	else
	{
		mXRotationAngle += angleToRotateBy;
	}
}

// ------------------------------------------------------------ //

void ThirdPersonCamera::CapToYRotationBounds(float& angleToRotateBy)
{
	if (mYRotationAngle + angleToRotateBy > -0.1f)
	{
		angleToRotateBy = -0.1f - mYRotationAngle;
		mYRotationAngle = -0.1f;
	}
	else if (mYRotationAngle + angleToRotateBy < -PIDIV4)
	{
		angleToRotateBy = -(fabsf(PIDIV4 - fabsf(mYRotationAngle)));
		mYRotationAngle = -PIDIV4;
	}
	else
	{
		mYRotationAngle += angleToRotateBy;
	}

}

// ------------------------------------------------------------ //