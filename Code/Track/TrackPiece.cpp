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
	// Now convert to a 0->128 format
	newPos.x += 64;
	newPos.y += 64;

	// Now convert the position to being multiple of the grid size
	int gridCellSize     = 8;
	int halfGridCellSize = gridCellSize / 2;

	newPos.x -= ((int)newPos.x % gridCellSize);
	newPos.y -= ((int)newPos.y % gridCellSize);
	
	// Offset the grid by half a cell size because the origin is in the centre of the grid
	newPos.x += halfGridCellSize;
	newPos.y += halfGridCellSize;

	// Now convert back to being the world space format
	newPos.x -= 64;
	newPos.y -= 64;

	// Floor the value to make it an integer, in the correct rounding direction
	newPos.x = floorf(newPos.x);
	newPos.y = floorf(newPos.y);

	// Cap to grid
	float worldSpaceQuaterGridCap = 64.0f - (float)halfGridCellSize;

	if (newPos.x > worldSpaceQuaterGridCap)
		newPos.x = worldSpaceQuaterGridCap;
	else if (newPos.x < -worldSpaceQuaterGridCap)
		newPos.x = -worldSpaceQuaterGridCap;

	if (newPos.y > worldSpaceQuaterGridCap)
		newPos.y = worldSpaceQuaterGridCap;
	else if (newPos.y < -worldSpaceQuaterGridCap)
		newPos.y = -worldSpaceQuaterGridCap;

	// Apply the new position
	mGridPosition.x = (int)newPos.x;
	mGridPosition.y = (int)newPos.y;

	DirectX::XMStoreFloat4x4(&mModelMatrix, DirectX::XMMatrixTranslation((float)mGridPosition.x, 0.0f, 
		                                                                 (float)mGridPosition.y));
}