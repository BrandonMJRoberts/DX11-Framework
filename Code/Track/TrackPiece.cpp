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