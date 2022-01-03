#ifndef _TRACK_PIECE_H
#define _TRACK_PIECE_H

#include <DirectXMath.h>

#include "../Camera/BaseCamera.h"
#include "../Maths/CommonMaths.h"

class Model;
class TrackCollision;

// -------------------------------------------------------------------- //

class TrackPiece final
{
public:
	TrackPiece(Model& model, Vector2D worldPosition);//, TrackCollision& collision);
	~TrackPiece();

	void RenderFull(BaseCamera* camera);
	void SetNewGridPosition(Vector2D newPos);

	Vector2D GetGridPosition();
	Vector2D GetGridPositionWorldSpace() { return mGridPosition; }

	static Vector2D ConvertFromWorldToGridPosition(Vector2D worldPos);
	static Vector2D ConvertFromGridToWorldPosition(Vector2D gridPos);

	void Rotate();

	unsigned int GetRotationAmount()                   { return mCurrentRotation / 90; }
	void         SetRotation(unsigned int newRotation);

private:
	//DirectX::XMFLOAT3 mFacingDirection;
	//TrackCollision&   mCollision;

	void UpdateModelMatrix();

	Vector2D            mGridPosition;
	unsigned int        mCurrentRotation;
	DirectX::XMFLOAT4X4 mModelMatrix;
	Model&              mModel;
};

// -------------------------------------------------------------------- //

#endif