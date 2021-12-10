#ifndef _BASE_CAMERA_H_
#define _BASE_CAMERA_H_

#include "../Maths/CommonMaths.h"

#include "../Input/InputHandler.h"

class BaseCamera abstract
{
public:
	         BaseCamera();
			 BaseCamera(InputHandler* inputHandler, 
				        Vector3D      startPos, 
				        Vector3D      right,  
				        Vector3D      up, 
				        float         FOV, 
				        float         nearPlane     = 0.01f,  
				        float         farPlane      = 100.0f, 
				        float         aspect        = 16.0f / 9.0f, 
				        float         movementSpeed = 1.0f,
				        float         rotationSpeed = 0.1f);
	virtual ~BaseCamera();

	virtual void Update(const float deltaTime);


	DirectX::XMFLOAT4X4 GetViewMatrix()        { return mViewMatrix; };
	virtual DirectX::XMFLOAT4X4 GetInverseViewMatrix() { return MatrixMaths::Identity4X4; } // Need to add

	DirectX::XMFLOAT4X4 GetPerspectiveMatrix() { return mPerspectiveMatrix; };
	virtual DirectX::XMFLOAT4X4 GetInversePerspectiveMatrix() { return MatrixMaths::Identity4X4; } // Need to add

	Vector3D GetPosition() { return mPosition; }
	virtual Vector3D GetFacingDirection() = 0;

	bool GetHasMovedFromLastFrame();

protected:
	virtual void ReCalculateViewMatrix();
	        void ReCalculatePerspectiveMatrix();

	InputHandler* mInputHandler;

	DirectX::XMFLOAT4X4 mViewMatrix;
	DirectX::XMFLOAT4X4 mPerspectiveMatrix;

	Vector3D mPosition;

	Vector3D mRight;
	Vector3D mUp;

	float    mFOV; // Stored in degrees
	float    mNearPlane;
	float    mFarPlane;

	float    mAspectRatio;
	float    mMovementSpeed;

	float    mRotationSpeed;

	bool     mHasMovedFromLastFrame;
};

#endif