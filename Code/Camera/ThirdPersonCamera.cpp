#include "ThirdPersonCamera.h"

// ------------------------------------------------------------------------- //

ThirdPersonCamera::ThirdPersonCamera()
	: BaseCamera()
	, mFocalPoint(0.0f, 0.0f, 0.0f)
	, mDistanceFromFocalPoint(10.0f)
{
	ReCalculatePosition();
}

// ------------------------------------------------------------------------- //

ThirdPersonCamera::ThirdPersonCamera(InputHandler* inputHandler, Vector3D focalPoint, float distanceFromFocalPoint, Vector3D right, Vector3D up, float FOV, float nearPlane, float farPlane, float aspect)
	: BaseCamera(inputHandler, Vector3D::zero, right, up, FOV, nearPlane, farPlane, aspect)
	, mFocalPoint(focalPoint)
	, mDistanceFromFocalPoint(distanceFromFocalPoint)
{
	ReCalculatePosition();
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
		// ---------------------------------------------------------------------------------------------------

		// Left/right
		if (mInputHandler->GetIsKeyPressed('A'))
		{
			mFocalPoint += (mRight * -mMovementSpeed) * deltaTime;
		}
		else if (mInputHandler->GetIsKeyPressed('D'))
		{
			mFocalPoint += (mRight * mMovementSpeed) * deltaTime;
		}

		// Up/down
		if (mInputHandler->GetIsKeyPressed(VK_SPACE))
		{
			mFocalPoint += (Vector3D::worldUp * mMovementSpeed) * deltaTime;
		}
		else if (mInputHandler->GetIsKeyPressed(VK_LSHIFT))
		{
			mFocalPoint += (Vector3D::worldUp * -mMovementSpeed) * deltaTime;
		}


		// As we need to restrict the movement to the X/Z plane we need to adjust the facing direction
		Vector3D facingDirection = mUp.Cross(mRight);
		facingDirection.y        = 0.0f;
		facingDirection.Normalise();

		// Now for forward/backward
		if (mInputHandler->GetIsKeyPressed('W'))
		{
			mFocalPoint += (facingDirection * mMovementSpeed) * deltaTime;
		}
		else if (mInputHandler->GetIsKeyPressed('S'))
		{
			mFocalPoint += (facingDirection * -mMovementSpeed) * deltaTime;
		}

		// Now re-calculate the position of the camera after the changes have been made
		ReCalculatePosition();

		// ---------------------------------------------------------------------------------------------------

		// Now check to see if the player is rotating the camera around the focal point

		// Check for the right mouse button press
		if (mInputHandler->GetIsMouseButtonPressed(1))
		{
			// Now check to see if the player is moving the mouse
			Vector2D mouseDelta = mInputHandler->GetMouseMovementDelta();

			// Rotate the facing direction around the world up by the rotation amount
			if (mouseDelta.x != 0.0f)
			{
				constexpr float angle = DirectX::XMConvertToRadians(45.0f);

				// Construct the rotation matrix
				DirectX::XMFLOAT3X3 rotationMatrix = {cosf(angle),  0.0f,  sinf(angle),
				                                      0.0f,         1.0f,  0.0f,
				                                      -sinf(angle), 0.0f,  cosf(angle)};

				// Calculate the offset vector
				Vector3D offsetVector = mPosition - mFocalPoint;

				// Now multiply the offset vector by the rotation matrix
				offsetVector *= rotationMatrix;

				// Now re-calculate the position
				mPosition = mFocalPoint + offsetVector;
			}

			// Rotate the view up/down around the local right axis
			if (mouseDelta.y != 0.0f)
			{
				// Construct the rotation matrix
				DirectX::XMFLOAT3X3 rotationMatrix = MatrixMaths::AxisRotationMatrix(mRight, 0.1f * deltaTime);

				// Calculate the offset vector
				Vector3D offsetVector = mPosition - mFocalPoint;

				// Apply the rotation to the offset
				offsetVector *= rotationMatrix;

				// Now re-calculate the position
				mPosition = mFocalPoint + offsetVector;
			}
		}

		// Now check to see if the player has scrolled the mouse wheel in/out
		int wheelDelta = mInputHandler->GetScrollWheelFrameDelta();
		if (wheelDelta != 0)
		{
			// Change the distance 
			if (wheelDelta > 0)
			{
				mDistanceFromFocalPoint += deltaTime * 2.0f;

				if (mDistanceFromFocalPoint > kMaxDistance)
					mDistanceFromFocalPoint = kMaxDistance;
			}
			else
			{
				mDistanceFromFocalPoint -= deltaTime * 2.0f;

				if (mDistanceFromFocalPoint < kMinDistance)
					mDistanceFromFocalPoint = kMinDistance;
			}
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
	Vector3D facingDir = mUp.Cross(mRight);

	// Now calculate the position
	mPosition = mFocalPoint - (facingDir.Normalised() * mDistanceFromFocalPoint);
}

// ------------------------------------------------------------ //