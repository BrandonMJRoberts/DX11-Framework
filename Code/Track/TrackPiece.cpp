#include "TrackPiece.h"

#include "../Models/Model.h"
#include "../Collisions/TrackCollision.h"

// -------------------------------------------------------------------- //

TrackPiece::TrackPiece(Model& model, Vector2D worldPosition)//, TrackCollision& collision)
	: mGridPosition(worldPosition)
	, mModel(model)
	, mModelMatrix(MatrixMaths::Identity4X4)
	, mCurrentRotation(0)
	//, mCollision(collision)
	//, mFacingDirection(0.0f, 0.0f, 0.0f)
{
	// Apply the position to the model matrix
	DirectX::XMStoreFloat4x4(&mModelMatrix, DirectX::XMMatrixTranslation((float)mGridPosition.x, 0.0f, 
		                                                                 (float)mGridPosition.y));
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
	mGridPosition.x = (float)(int)newPos.x;
	mGridPosition.y = (float)(int)newPos.y;

	UpdateModelMatrix();
}

// --------------------------------------------------------------------------------------------------------- //

Vector2D TrackPiece::GetGridPosition()
{
	// Convert the world space grid position that is used for rendering the model into a grid space in the internal grid
	// The current bounds are from -60 to +60 (with a grid cell size of 8)
	Vector2D returnValue = mGridPosition;

	// Dividing by the cell width gives the current position of the piece in grid space
	// This will always be a number + 0.5 on each axis due to the offset
	returnValue.x /= 8;
	returnValue.y /= 8;

	// Take off that offset
	returnValue.x -= 0.5f;
	returnValue.y -= 0.5f;

	// Now add on the 8 (the cutoff half of the entire grid that lies in the negative section), to get the true grid position
	returnValue.x += 8;
	returnValue.y += 8;

	return returnValue;
}

// --------------------------------------------------------------------------------------------------------- //

// This converts from any world position into a grid position
Vector2D TrackPiece::ConvertFromWorldToGridPosition(Vector2D worldPos)
{
	// First convert to a 0->128 format
	worldPos.x += 64;
	worldPos.y += 64;

	// Now convert the position to being multiple of the grid size
	int gridCellSize     = 8;
	int halfGridCellSize = gridCellSize / 2;

	// Make the position a multiple of the cell size
	worldPos.x -= ((int)worldPos.x % gridCellSize);
	worldPos.y -= ((int)worldPos.y % gridCellSize);

	// Offset the grid by half a cell size because the origin is in the centre of the grid
	worldPos.x += halfGridCellSize;
	worldPos.y += halfGridCellSize;

	// Now convert back to being the world space format
	worldPos.x -= 64;
	worldPos.y -= 64;

	// Floor the value to make it an integer, in the correct rounding direction
	worldPos.x = floorf(worldPos.x);
	worldPos.y = floorf(worldPos.y);

	// Cap to grid
	float worldSpaceQuaterGridCap = 64.0f - (float)halfGridCellSize;

	if (worldPos.x > worldSpaceQuaterGridCap)
		worldPos.x = worldSpaceQuaterGridCap;
	else if (worldPos.x < -worldSpaceQuaterGridCap)
		worldPos.x = -worldSpaceQuaterGridCap;

	if (worldPos.y > worldSpaceQuaterGridCap)
		worldPos.y = worldSpaceQuaterGridCap;
	else if (worldPos.y < -worldSpaceQuaterGridCap)
		worldPos.y = -worldSpaceQuaterGridCap;

	// Now the position is obaying the grid, with cell width of 8, between the values of -60 to +60
	// So now convert to the format of 0->15
	worldPos.x /= 8;
	worldPos.y /= 8;

	// Take off that offset
	worldPos.x -= 0.5f;
	worldPos.y -= 0.5f;

	// Now add on the 8 (the cutoff half of the entire grid that lies in the negative section), to get the true grid position
	worldPos.x += 8;
	worldPos.y += 8;

	return worldPos;
}

// --------------------------------------------------------------------------------------------------------- //

Vector2D TrackPiece::ConvertFromGridToWorldPosition(Vector2D gridPos)
{
	// Passes in the grid position in the range 0 -> 15 on each axis
	// Need to convert it to the format of -60 to +60
	// First minus off 8
	gridPos.x -= 8;
	gridPos.y -= 8;

	// Now add back on 0.5 for the offset to the centre of the grid
	gridPos.x += 0.5f;
	gridPos.y += 0.5f;

	// Now multiply by 8 to get the world pos
	gridPos *= 8;

	return gridPos;
}

// --------------------------------------------------------------------------------------------------------- //

void TrackPiece::Rotate()
{
	// Rotate 90 degrees around
	mCurrentRotation += 90;

	if (mCurrentRotation >= 360)
		mCurrentRotation = 0;

	UpdateModelMatrix();
}

// --------------------------------------------------------------------------------------------------------- //

void TrackPiece::SetRotation(unsigned int newRotation) 
{ 
	mCurrentRotation = newRotation; 

	UpdateModelMatrix();
}

// --------------------------------------------------------------------------------------------------------- //

void TrackPiece::UpdateModelMatrix()
{
	DirectX::XMMATRIX rotationMatrix    = DirectX::XMMatrixRotationAxis(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), DirectX::XMConvertToRadians((float)mCurrentRotation));
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation((float)mGridPosition.x, 0.0f, (float)mGridPosition.y);
	//DirectX::XMMATRIX scaleMatrix       = DirectX::XMMatrixScaling(3.0f, 3.0f, 3.0f);

	// Apply the rotation to the model matrix
	DirectX::XMStoreFloat4x4(&mModelMatrix, rotationMatrix * translationMatrix);
}

// --------------------------------------------------------------------------------------------------------- //