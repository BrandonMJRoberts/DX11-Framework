#ifndef _THIRD_PERSON_CAMERA_H_
#define _THIRD_PERSON_CAMERA_H_

#include "BaseCamera.h"
#include "../Shaders/ShaderHandler.h"

class Model;
class ShaderHandler;

#define CAMERA_UP_TILT_MAX (2.0f * (PI / 3.0f))
#define CAMERA_UP_TILT_MIN (5.0f * (PI / 9.0f))

class ThirdPersonCamera : public BaseCamera
{
public:
	/*ThirdPersonCamera();*/
	ThirdPersonCamera(InputHandler* inputHandler, 
		              ShaderHandler& shaderHandler,
		              Vector3D      focalPoint, 
		              float         distanceFromFocalPoint, 
		              Vector3D      right, 
		              Vector3D      up,  
		              float         FOV, 
		              float         nearPlane     = 0.01f, 
		              float         farPlane      = 100.0f, 
		              float         aspect        = 16.0f / 9.0f, 
		              float         movementSpeed = 1.0f,
		              float         rotationSpeed = 0.2f);
	~ThirdPersonCamera() override;

	void Update(const float deltaTime) override;

	void     ReCalculateViewMatrix() override;
	Vector3D GetFacingDirection()    override;

	void     RenderFocusPoint();

	DirectX::XMFLOAT4X4 GetInverseViewMatrix()        override;
	DirectX::XMFLOAT4X4 GetInversePerspectiveMatrix() override;

private:
	void ReCalculatePosition();

	void MouseWheelCheck(bool& changed, const float deltaTime);
	void MovementCheck(  bool& changed, const float deltaTime);
	void RotationalCheck(bool& changed, const float deltaTime);

	void CapToXRotationBounds(float& angleToRotateBy);
	void CapToYRotationBounds(float& angleToRotateBy);

	Model*              mFocusPointIcon;
	DirectX::XMFLOAT4X4 mFocusModelMatrix;

	ShaderHandler&      mShaderHandler;
	ID3D11BlendState*   mTransparencyBlendState;

	const float kMaxDistance = 50.0f;
	const float kMinDistance = 12.0f;

	Vector3D mFocalPoint;
	float    mDistanceFromFocalPoint;

	float    mXRotationAngle;
	float    mYRotationAngle;
};

#endif