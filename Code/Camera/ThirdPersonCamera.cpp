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

		//// Left/right
		//if (mInputHandler->GetIsKeyPressed(DIK_A))
		//{
		//	mFocalPoint += (mRight * -mMovementSpeed) * deltaTime;
		//}
		//else if (mInputHandler->GetIsKeyPressed(DIK_D))
		//{
		//	mFocalPoint += (mRight * mMovementSpeed) * deltaTime;
		//}

		//// Up/down
		//if (mInputHandler->GetIsKeyPressed(DIK_SPACE))
		//{
		//	mFocalPoint += (Vector3D::worldUp * mMovementSpeed) * deltaTime;
		//}
		//else if (mInputHandler->GetIsKeyPressed(DIK_LSHIFT))
		//{
		//	mFocalPoint += (Vector3D::worldUp * -mMovementSpeed) * deltaTime;
		//}


		//// As we need to restrict the movement to the X/Z plane we need to adjust the facing direction
		//Vector3D facingDirection = mUp.Cross(mRight);
		//facingDirection.y        = 0.0f;
		//facingDirection.Normalise();

		//// Now for forward/backward
		//if (mInputHandler->GetIsKeyPressed(DIK_W))
		//{
		//	mFocalPoint += (facingDirection * mMovementSpeed) * deltaTime;
		//}
		//else if (mInputHandler->GetIsKeyPressed(DIK_S))
		//{
		//	mFocalPoint += (facingDirection * -mMovementSpeed) * deltaTime;
		//}

		// Now re-calculate the position of the camera after the changes have been made
		ReCalculatePosition();

		// ---------------------------------------------------------------------------------------------------

		// Now check to see if the player is rotating the camera around the focal point

		//if(mInputHandler->)

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