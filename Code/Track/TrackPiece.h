#ifndef _TRACK_PIECE_H
#define _TRACK_PIECE_H

#include <DirectXMath.h>

class Model;
class TrackCollision;

// -------------------------------------------------------------------- //

class TrackPiece final
{
public:
	TrackPiece(Model& model, TrackCollision& collision);
	~TrackPiece();

	void RenderGeometry();
	void RenderFull();

private:
	DirectX::XMUINT3  mGridPosition;
	DirectX::XMFLOAT3 mFacingDirection;

	Model&            mModel;
	TrackCollision&   mCollision;
};

// -------------------------------------------------------------------- //

#endif