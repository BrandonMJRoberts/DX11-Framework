#include "ThirdPersonCamera.h"

#include "../GameScreens/ScreenManager.h"
#include "../Models/Model.h"

// ------------------------------------------------------------------------- //
//
//ThirdPersonCamera::ThirdPersonCamera()
//	: BaseCamera()
//	, mFocalPoint(0.0f, 0.0f, 0.0f)
//	, mDistanceFromFocalPoint(10.0f)
//	, mYRotationAngle(0.0f)
//	, mXRotationAngle(0.0f)
//	, mFocusPointIcon(nullptr)
//	, mFocusModelMatrix(MatrixMaths::Identity4X4)
//	, mTransparencyBlendState(nullptr)
//{
//	float zero = 0.0f;
//	CapToYRotationBounds(zero);
//	CapToXRotationBounds(zero);
//
//	ReCalculatePosition();
//
//	ReCalculatePerspectiveMatrix();
//	ReCalculateViewMatrix();
//}

// ------------------------------------------------------------------------- //

ThirdPersonCamera::ThirdPersonCamera(InputHandler* inputHandler,
									 ShaderHandler& shaderHandler,
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
	, mFocusPointIcon(nullptr)
	, mFocusModelMatrix(MatrixMaths::Identity4X4)
	, mTransparencyBlendState(nullptr)
	, mShaderHandler(shaderHandler)
{
	float zero = 0.0f;
	CapToYRotationBounds(zero);
	CapToXRotationBounds(zero);

	ReCalculatePosition();

	ReCalculatePerspectiveMatrix();
	ReCalculateViewMatrix();

	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader*  pixelShader  = nullptr;

	//VertexShaderReturnData returnData = shaderHandler.CompileVertexShader(L"ModelLightingRender.fx", "VS");
	  VertexShaderReturnData returnData = shaderHandler.CompileVertexShader(L"ModelGhostlyRender.fx", "VS");
	                       vertexShader = returnData.vertexShader;

						 //pixelShader = shaderHandler.CompilePixelShader(L"ModelLightingRender.fx", "PS");
						   pixelShader = shaderHandler.CompilePixelShader(L"ModelGhostlyRender.fx", "PS");

	mFocusPointIcon = new Model(shaderHandler, "Models/CameraFocus/CameraSphere.obj", vertexShader, pixelShader, returnData.Blob, true);

	shaderHandler.CreateBlendState(&mTransparencyBlendState, 
		                            true,  
		                            D3D11_BLEND_SRC_COLOR, 
		                            D3D11_BLEND_BLEND_FACTOR,  
		                            D3D11_BLEND_OP_ADD, 
		                            D3D11_BLEND_ONE,
		                            D3D11_BLEND_ZERO, 
		                            D3D11_BLEND_OP_ADD, 
		                            D3D11_COLOR_WRITE_ENABLE_ALL);
}

// ------------------------------------------------------------------------- //

ThirdPersonCamera::~ThirdPersonCamera()
{
	delete mFocusPointIcon;
	mFocusPointIcon = nullptr;
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

	// Cap the point to the playable area
	if (mFocalPoint.x > 64.0f)
		mFocalPoint.x = 64.0f;
	else if (mFocalPoint.x < -64.0f)
		mFocalPoint.x = -64.0f;

	if (mFocalPoint.z > 64.0f)
		mFocalPoint.z = 64.0f;
	else if (mFocalPoint.z < -64.0f)
		mFocalPoint.z = -64.0f;

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

Vector3D ThirdPersonCamera::GetFacingDirection()
{
	return (mFocalPoint - mPosition).Normalised();
}

// ------------------------------------------------------------ //

void ThirdPersonCamera::RenderFocusPoint()
{
	if (mFocusPointIcon)
	{
		Vector3D facingDir = GetFacingDirection();

		float t = -mPosition.y / facingDir.y;

		if (t < 0.0f)
			return;

		Vector2D newPos = Vector2D(mPosition.x + (t * facingDir.x), 
								   mPosition.z + (t * facingDir.z));

		// Calculate the position of the focus point
		float scale = 0.5f;
		DirectX::XMStoreFloat4x4(&mFocusModelMatrix, DirectX::XMMatrixScaling(scale, scale, scale) * DirectX::XMMatrixTranslation(newPos.x, 0.0f, newPos.y));

		// Bind the transparency blend state
		float blendFactor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
		mShaderHandler.BindBlendState(mTransparencyBlendState, blendFactor);

		mFocusPointIcon->FullRender(this, mFocusModelMatrix);

		// Re-bind the default transparent blend state
		mShaderHandler.BindDefaultBlendState();
	}
}

// ------------------------------------------------------------ //

DirectX::XMFLOAT4X4 ThirdPersonCamera::GetInverseViewMatrix()
{
	DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(&mViewMatrix);
	DirectX::XMVECTOR det     = DirectX::XMMatrixDeterminant(viewMat);

	DirectX::XMFLOAT4X4 returnMat;
	DirectX::XMStoreFloat4x4(&returnMat, DirectX::XMMatrixInverse(&det, viewMat));

	return returnMat;
}

// ------------------------------------------------------------ //

DirectX::XMFLOAT4X4 ThirdPersonCamera::GetInversePerspectiveMatrix()
{
	DirectX::XMMATRIX perspMat = DirectX::XMLoadFloat4x4(&mPerspectiveMatrix);
	DirectX::XMVECTOR det      = DirectX::XMMatrixDeterminant(perspMat);

	DirectX::XMFLOAT4X4 returnMat;
	DirectX::XMStoreFloat4x4(&returnMat, DirectX::XMMatrixInverse(&det, perspMat));

	return returnMat;
}

// ------------------------------------------------------------ //