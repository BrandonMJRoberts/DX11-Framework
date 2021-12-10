#include "BaseCamera.h"

// ------------------------------------------------------------ //

BaseCamera::BaseCamera()
	: mViewMatrix()
	, mPosition(0.0f, 0.0f, 0.0f)
	, mRight(1.0f, 0.0f, 0.0)
	, mUp(0.0f, 1.0f, 0.0f)
	, mFOV(50.0f)
	, mNearPlane(0.01f)
	, mFarPlane(100.0f)
	, mAspectRatio(16.0f / 9.0f)
	, mPerspectiveMatrix()
	, mInputHandler(nullptr)
	, mMovementSpeed(0.1f)
	, mRotationSpeed(0.1f)
	, mHasMovedFromLastFrame(false)
{
	//ReCalculateViewMatrix();
	//ReCalculatePerspectiveMatrix();
}

// ------------------------------------------------------------ //

BaseCamera::BaseCamera(InputHandler* inputHandler, Vector3D startPos, Vector3D right, Vector3D up, float FOV, float nearPlane, float farPlane, float aspect, float movementSpeed, float rotationSpeed)
	: mViewMatrix()
	, mPosition(startPos)
	, mRight(right.Normalise())
	, mUp(up.Normalise())
	, mFOV(FOV)
	, mNearPlane(nearPlane)
	, mFarPlane(farPlane)
	, mAspectRatio(aspect)
	, mPerspectiveMatrix()
	, mInputHandler(inputHandler)
	, mMovementSpeed(movementSpeed)
	, mRotationSpeed(rotationSpeed)
	, mHasMovedFromLastFrame(false)
{
	//ReCalculateViewMatrix();
	//ReCalculatePerspectiveMatrix();
}

// ------------------------------------------------------------ //

BaseCamera::~BaseCamera()
{
	mInputHandler = nullptr;
}

// ------------------------------------------------------------ //

void BaseCamera::Update(const float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);
}

// ------------------------------------------------------------ //

void BaseCamera::ReCalculateViewMatrix()
{
	// Calculate view matrix
	Vector3D viewDir    = mRight.Cross(mUp);
	Vector3D focalPoint = mPosition + viewDir;

	DirectX::XMStoreFloat4x4(&mViewMatrix, DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(mPosition.x, mPosition.y, mPosition.z, 0.0f), 
																	 DirectX::XMVectorSet(focalPoint.x, focalPoint.y, focalPoint.z, 0.0f), 
																	 DirectX::XMVectorSet(mUp.x, mUp.y, mUp.z, 0.0f)));
}

// ------------------------------------------------------------ //

void BaseCamera::ReCalculatePerspectiveMatrix()
{
	// We are going to be mainly using perspective so re-calculate the view matrix of a perspective view
	DirectX::XMStoreFloat4x4(&mPerspectiveMatrix, DirectX::XMMatrixPerspectiveFovLH(mFOV, mAspectRatio, mNearPlane, mFarPlane));
}

// ------------------------------------------------------------ //

bool BaseCamera::GetHasMovedFromLastFrame()
{
	bool hasMoved = mHasMovedFromLastFrame;

	mHasMovedFromLastFrame = false;

	return hasMoved;
}