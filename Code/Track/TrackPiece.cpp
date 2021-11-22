#include "TrackPiece.h"

#include "../Models/Model.h"
#include "../Collisions/TrackCollision.h"

// -------------------------------------------------------------------- //

TrackPiece::TrackPiece(Model& model)//, TrackCollision& collision)
	: mGridPosition(0, 0)
	, mFacingDirection(0.0f, 0.0f, 0.0f)
	, mModel(model)
	//, mCollision(collision)
	, mModelMatrix(MatrixMaths::Identity4X4)
{

}

// -------------------------------------------------------------------- //

TrackPiece::~TrackPiece()
{

}

// -------------------------------------------------------------------- //

void TrackPiece::RenderFull(BaseCamera* camera)
{
	mModel.FullRender(camera, mModelMatrix);
}

// -------------------------------------------------------------------- //

void TrackPiece::SetNewGridPosition(Vector2D newPos)
{
	mGridPosition.x = newPos.x; 
	mGridPosition.y = newPos.y;

	DirectX::XMStoreFloat4x4(&mModelMatrix, DirectX::XMMatrixTranslation(mGridPosition.x * 4.0f, 0.0f, mGridPosition.y * 4.0f));

	//mModelMatrix = DirectX::XMMatrixTranslation(mGridPosition.x * 4.0f, 0.0f, mGridPosition.y * 4.0f);
}