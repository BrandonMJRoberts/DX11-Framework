#include "ThirdPersonCamera.h"

// ------------------------------------------------------------------------- //

ThirdPersonCamera::ThirdPersonCamera()
	: BaseCamera()
	, mFocalPoint(0.0f, 0.0f, 0.0f)
	, mDistanceFromFocalPoint(10.0f)
{
	ReCalculatePosition();

	ReCalculatePerspectiveMatrix();
	ReCalculateViewMatrix();
}

// ------------------------------------------------------------------------- //

ThirdPersonCamera::ThirdPersonCamera(InputHandler* inputHandler, Vector3D focalPoint, float distanceFromFocalPoint, Vector3D right, Vector3D up, float FOV, float nearPlane, float farPlane, float aspect, float movementSpeed, float rotationSpeed)
	: BaseCamera(inputHandler, Vector3D::zero, right, up, FOV, nearPlane, farPlane, aspect, movementSpeed, rotationSpeed)
	, mFocalPoint(focalPoint)
	, mDistanceFromFocalPoint(distanceFromFocalPoint)
{
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

		// ---------------------------------------------------------------------------------------------------

		// Left/right
		if (mInputHandler->GetIsKeyPressed('A'))
		{
			changed      = true;
			mFocalPoint += (mRight * -mMovementSpeed) * deltaTime;
		}
		else if (mInputHandler->GetIsKeyPressed('D'))
		{
			changed      = true;
			mFocalPoint += (mRight * mMovementSpeed) * deltaTime;
		}

		// Up/down
		if (mInputHandler->GetIsKeyPressed(VK_SPACE))
		{
			changed      = true;
			mFocalPoint += (Vector3D::worldUp * mMovementSpeed) * deltaTime;
		}
		else if (mInputHandler->GetIsKeyPressed('X'))
		{
			changed      = true;
			mFocalPoint += (Vector3D::worldUp * -mMovementSpeed) * deltaTime;
		}


		// As we need to restrict the movement to the X/Z plane we need to adjust the facing direction
		Vector3D facingDirection = mRight.Cross(mUp);
		facingDirection.y        = 0.0f;
		facingDirection.Normalise();

		// Now for forward/backward
		if (mInputHandler->GetIsKeyPressed('W'))
		{
			changed      = true;
			mFocalPoint += (facingDirection * mMovementSpeed) * deltaTime;
		}
		else if (mInputHandler->GetIsKeyPressed('S'))
		{
			changed      = true;
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

			// If the mouse is moved at all
			if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)
			{
				changed = true;

				// Calculate the offset vector
				Vector3D offsetVector = mPosition - mFocalPoint;

				DirectX::XMMATRIX xMovementMatrix = DirectX::XMLoadFloat4x4(&MatrixMaths::Identity4X4);
				DirectX::XMMATRIX yMovementMatrix = DirectX::XMLoadFloat4x4(&MatrixMaths::Identity4X4);

				// Now calculate the rotation matrixes for the two rotations
				if (mouseDelta.x > 0.0f)
				{
					xMovementMatrix = DirectX::XMLoadFloat4x4(&MatrixMaths::Convert3X3To4X4(MatrixMaths::GetYAxisRotationMatrix(mRotationSpeed * deltaTime)));
				}
				else if (mouseDelta.x < 0.0f)
				{
					xMovementMatrix = DirectX::XMLoadFloat4x4(&MatrixMaths::Convert3X3To4X4(MatrixMaths::GetYAxisRotationMatrix(-mRotationSpeed * deltaTime)));
				}

				if(mouseDelta.y > 0.0f)
				{
					yMovementMatrix = DirectX::XMLoadFloat4x4(&MatrixMaths::Convert3X3To4X4(MatrixMaths::AxisRotationMatrix(mRight, mRotationSpeed * deltaTime)));
				}
				else if (mouseDelta.y < 0.0f)
				{
					yMovementMatrix = DirectX::XMLoadFloat4x4(&MatrixMaths::Convert3X3To4X4(MatrixMaths::AxisRotationMatrix(mRight, -mRotationSpeed * deltaTime)));
				}

				// Now apply the multiplication
				xMovementMatrix = DirectX::XMMatrixMultiply(xMovementMatrix, yMovementMatrix);

				// Convert back and apply the rotation
				DirectX::XMFLOAT4X4 finalMatrix;
				DirectX::XMStoreFloat4x4(&finalMatrix, xMovementMatrix);

				offsetVector *= MatrixMaths::Convert4X4RotationMatrixTo3X3(finalMatrix);

				// Recalculate the position of the camera
				mPosition = mFocalPoint + offsetVector;

				// Re-calculate the right vector
				mRight = mUp.Cross(offsetVector.Normalise());
			}

			// Rotate the facing direction around the world up by the rotation amount
			//if (mouseDelta.x != 0.0f)
			//{
			//	changed = true;

			//	// Calculate the offset vector
			//	Vector3D offsetVector = mPosition - mFocalPoint;

			//	// Now multiply the offset vector by the rotation matrix
			//	offsetVector *= MatrixMaths::GetYAxisRotationMatrix(mRotationSpeed * deltaTime);

			//	// Now re-calculate the position
			//	mPosition = mFocalPoint + offsetVector;
			//}

			//// Rotate the view up/down around the local right axis
			//if (mouseDelta.y != 0.0f)
			//{
			//	changed = true;

			//	// Calculate the offset vector
			//	Vector3D offsetVector = mPosition - mFocalPoint;

			//	// Apply the rotation to the offset
			//	offsetVector *= MatrixMaths::AxisRotationMatrix(mRight, mRotationSpeed * deltaTime);

			//	// Now re-calculate the position
			//	mPosition = mFocalPoint + offsetVector;
			//}
		}

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