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
#include "../Maths/Constants.h"
void TrackPiece::SetNewGridPosition(Vector2D newPos)
{
	// Cap to grid
	if (newPos.x > 64.0f)
		newPos.x = 64.0f;
	else if (newPos.x < -64.0f)
		newPos.x = -64.0f;

	if (newPos.y > 64.0f)
		newPos.y = 64.0f;
	else if (newPos.y < -64.0f)
		newPos.y = -64.0f;

	// Now convert to a 0->128 format
	newPos.x += 64;
	newPos.y += 64;

	// Now convert the position to being multiple of the grid size
	unsigned int gridCellSize = 8;
	newPos.x -= ((int)newPos.x % gridCellSize);
	newPos.y -= ((int)newPos.y % gridCellSize);
	
	newPos.x += gridCellSize / 2;
	newPos.y += gridCellSize / 2;

	// Now convert back to being the world space format
	newPos.x -= 64;
	newPos.y -= 64;

	mGridPosition.x = floor(newPos.x); 
	mGridPosition.y = floor(newPos.y);

	DirectX::XMStoreFloat4x4(&mModelMatrix, DirectX::XMMatrixTranslation(mGridPosition.x, 0.0f, 
		                                                                 mGridPosition.y));
}