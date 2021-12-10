#include "Grid.h"

#include "../../Camera/BaseCamera.h"
#include "../../GameScreens/ScreenManager.h"

#include <fstream>

// ------------------------------------------------------------------------ //

EditorGrid::EditorGrid(ShaderHandler& shaderHandler)
	: mShaderHandler(shaderHandler)
	, mBlendState(nullptr)
	, mAddPieceState(true)
	, mSwappedState(false)
{
	// Make sure that the grid is empty
	ClearGrid();

	// Make sure to initialise the factory
	TrackPieceFactory::GetInstance()->Init(&shaderHandler);

	// Create the highlight object that will change as new track types are selected
	mPotentialNewPiece.pieceType  = TrackPieceType::GHOST;
	mPotentialNewPiece.trackPiece = TrackPieceFactory::GetInstance()->CreateTrackPiece(TrackPieceType::GHOST, Vector2D::zero);

	// Create the transparency blend state
	mShaderHandler.CreateBlendState(&mBlendState, 
		                            true,  
		                            D3D11_BLEND_SRC_COLOR, 
		                            D3D11_BLEND_BLEND_FACTOR,  
		                            D3D11_BLEND_OP_ADD, 
		                            D3D11_BLEND_ONE,
		                            D3D11_BLEND_ZERO, 
		                            D3D11_BLEND_OP_ADD, 
		                            D3D11_COLOR_WRITE_ENABLE_ALL);
}

// ------------------------------------------------------------------------ //

EditorGrid::EditorGrid(ShaderHandler& shaderHandler, std::string filePathForTrackLoad)
	: mShaderHandler(shaderHandler)
	, mSwappedState(false)
	, mAddPieceState(true)
{
	// Make sure that the grid is empty
	ClearGrid();

	LoadInGridFromFile(filePathForTrackLoad);
}

// ------------------------------------------------------------------------ //

EditorGrid::~EditorGrid()
{
	ClearGrid();
}

// ------------------------------------------------------------------------ //

GridPiece EditorGrid::GetTrackPiece(unsigned int x, unsigned int y)
{
	return mGrid[x][y];
}

// ------------------------------------------------------------------------ //

GridPiece EditorGrid::GetTrackPiece(Vector2D position)
{
	return mGrid[(unsigned int)position.x][(unsigned int)position.y];
}

// ------------------------------------------------------------------------ //

void      EditorGrid::SetGridPiece(TrackPieceType typeToAdd, Vector2D position)
{
	// First check to see if the piece is not already the same thing
	if (typeToAdd == GetTrackPiece(position).pieceType)
		return;

	if (typeToAdd == TrackPieceType::EMPTY)
	{
		// If we are setting this point to being empty, and it is not already empty then we need to clear up some memory
		delete mGrid[(unsigned int)position.x][(unsigned int)position.y].trackPiece;
		mGrid[(unsigned int)position.x][(unsigned int)position.y].trackPiece = nullptr;

		mGrid[(unsigned int)position.x][(unsigned int)position.y].pieceType  = TrackPieceType::EMPTY;
		return;
	}

	mGrid[(unsigned int)position.x][(unsigned int)position.y].pieceType  = typeToAdd;
	mGrid[(unsigned int)position.x][(unsigned int)position.y].trackPiece = TrackPieceFactory::GetInstance()->CreateTrackPiece(typeToAdd, TrackPiece::ConvertFromGridToWorldPosition(position));
}

// ------------------------------------------------------------------------ //

void      EditorGrid::SetGridPiece(TrackPieceType typeToAdd, unsigned int x, unsigned int y)
{
	if (typeToAdd == mGrid[x][y].pieceType)
		return;

	// Set the new type
	mGrid[x][y].pieceType  = typeToAdd;

	// Assign the track piece
	mGrid[x][y].trackPiece = TrackPieceFactory::GetInstance()->CreateTrackPiece(typeToAdd, TrackPiece::ConvertFromGridToWorldPosition(Vector2D((float)x, (float)y)));
}

// ------------------------------------------------------------------------ //

void EditorGrid::ClearGrid()
{
	for (unsigned int i = 0; i < Constants::GRID_WIDTH; i++)
	{
		for (unsigned int j = 0; j < Constants::GRID_HEIGHT; j++)
		{
			mGrid[i][j].pieceType  = TrackPieceType::EMPTY;
			mGrid[i][j].trackPiece = nullptr;	
		}
	}
}

// ------------------------------------------------------------------------ //

void EditorGrid::Update(const float deltaTime, InputHandler& inputHandler)
{
	UNREFERENCED_PARAMETER(deltaTime);

	// Check to see if the player wants to switch state between adding and removing
	if (inputHandler.GetIsMouseButtonPressed(2))
	{
		if (!mSwappedState)
		{
			mAddPieceState = !mAddPieceState;
			mSwappedState  = true;
		}
	}
	else
	{
		mSwappedState = false;
	}

	// Check to see if the player has clicked the left mouse button
	if (inputHandler.GetIsMouseButtonPressed(0))
	{
		Vector2D gridPos;

		// If in the create mode then add a piece
		if(mAddPieceState)
		{
			if (!mPotentialNewPiece.trackPiece)
				return;

			// Place the new piece in the internal grid where the highlight shows
			gridPos = mPotentialNewPiece.trackPiece->GetGridPosition();
			SetGridPiece(mPotentialNewPiece.pieceType, gridPos);
		}
		else
		{
			// If the player presses the right mouse button over a piece then it is removed
			gridPos = mPotentialNewPiece.trackPiece->GetGridPosition();
			SetGridPiece(TrackPieceType::EMPTY, gridPos);
		}
	}
}

// ------------------------------------------------------------------------ //

void EditorGrid::Render(BaseCamera* camera, InputHandler& inputHandler)
{
	// If the camera has moved then we will need to re-calculate which grid pieces are visible to the camera
	if (camera)
	{
		if (camera->GetHasMovedFromLastFrame())
		{
			FindAllVisibleGridPieces(camera);
		}
	}

	// Loop through all grid pieces that are visible to the camera
	for (unsigned int i = 0; i < mVisiblePieces.size(); i++)
	{
		if (mVisiblePieces[i].trackPiece)
		{
			if (mVisiblePieces[i].pieceType == TrackPieceType::GHOST)
			{
				float blendFactor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
				mShaderHandler.BindBlendState(mBlendState, blendFactor);

				// Render the piece
				mVisiblePieces[i].trackPiece->RenderFull(camera);

				// Bind the default state again
				mShaderHandler.BindDefaultBlendState();

				continue;
			}

			// Render the piece
			mVisiblePieces[i].trackPiece->RenderFull(camera);
		}
	}


	//for (unsigned int i = 0; i < Constants::GRID_WIDTH; i++)
	//{
	//	for (unsigned int j = 0; j < Constants::GRID_HEIGHT; j++)
	//	{
	//		if (mGrid[i][j].trackPiece)
	//		{
	//			if (mGrid[i][j].pieceType == TrackPieceType::GHOST)
	//			{
	//				float blendFactor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	//				mShaderHandler.BindBlendState(mBlendState, blendFactor);

	//				// Render the piece
	//				mGrid[i][j].trackPiece->RenderFull(camera);

	//				// Bind the default state again
	//				mShaderHandler.BindDefaultBlendState();

	//				continue;
	//			}

	//			// Render the piece
	//			mGrid[i][j].trackPiece->RenderFull(camera);
	//		}
	//	}
	//}

	// ---------------------------------------------------------------------------------------------------------------

	// We need to also render the potential new placment piece based off of where the player is hovering over
	if (!camera || !mPotentialNewPiece.trackPiece)
		return;

	// Get the mouse position on the screen
	Vector2D mousePos = inputHandler.GetMousePosition();

	if (inputHandler.GetIsKeyPressed('P'))
	{
		int a = 0;
		a++;
	}

	// First convert the mouse pos into NDC
	Vector2D normalisedDeviceCoords = Vector2D((2.0f * mousePos.x) / GameScreenManager::ScreenWidth - 1.0f,
		                                        1.0f - (2.0f * mousePos.y) / GameScreenManager::ScreenHeight);

	if (normalisedDeviceCoords.x < -1.0f || normalisedDeviceCoords.x > 1.0f ||
-		normalisedDeviceCoords.y < -1.0f || normalisedDeviceCoords.y > 1.0f)
	{
		return;
	}

	// Now convert the projection space point into view space
	DirectX::XMFLOAT4X4 projectionMat = camera->GetPerspectiveMatrix();

	// Get the ray in view space
	normalisedDeviceCoords.x   /= projectionMat(0, 0);
	normalisedDeviceCoords.y   /= projectionMat(1, 1);

	// Define the ray origin and direction in view space
	DirectX::XMVECTOR rayOrigin = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMVECTOR rayDir    = DirectX::XMVectorSet(normalisedDeviceCoords.x, normalisedDeviceCoords.y, 1.0f, 0.0f);

	// Now convert the ray dir and origin into world space by applying the inverse view matrix
	DirectX::XMFLOAT4X4 inverseView = camera->GetInverseViewMatrix();
	DirectX::XMMATRIX   invView     = DirectX::XMLoadFloat4x4(&inverseView);

	rayOrigin = DirectX::XMVector3TransformCoord(rayOrigin, invView);
	rayDir    = DirectX::XMVector3TransformNormal(rayDir, invView);

	// Normalise the direction vector
	rayDir    = DirectX::XMVector3Normalize(rayDir);

	// Convert to useful variables
	DirectX::XMFLOAT4 origin, direction;
	DirectX::XMStoreFloat4(&origin, rayOrigin);
	DirectX::XMStoreFloat4(&direction, rayDir);

	// Now we have the ray origin and direction in world space, so check for the intersection with the ground

	// Calculate the time factor
	float t = (-origin.y) / direction.y;

	Vector2D gridPos;

	// Set the new position
	gridPos.x = origin.x + (t * direction.x);
	gridPos.y = origin.z + (t * direction.z);

	mPotentialNewPiece.trackPiece->SetNewGridPosition(gridPos);

	// ---------------------------------------------------------------------------------------------------------------

	// Bind the transparency state before rendering 

	float blendFactor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	mShaderHandler.BindBlendState(mBlendState, blendFactor);

	mPotentialNewPiece.trackPiece->RenderFull(camera);

	// Re-bind the normal blend state
	mShaderHandler.BindDefaultBlendState();
}

// ------------------------------------------------------------------------ //

void EditorGrid::LoadInGridFromFile(std::string filePath)
{

}

// ------------------------------------------------------------------------ //

void EditorGrid::SaveOutTrackInternalDataToFile()
{

}

// ------------------------------------------------------------------------ //

void EditorGrid::FindAllVisibleGridPieces(BaseCamera* camera)
{
	// Loop through the entire grid and see if it is visible to the camera
	mVisiblePieces.clear();

	// For now just add all actual pieces to the list
	for(unsigned int i = 0; i < Constants::GRID_WIDTH; i++)
	{
		for (unsigned int j = 0; j < Constants::GRID_HEIGHT; j++)
		{
			if (mGrid[i][j].pieceType != TrackPieceType::EMPTY)
				mVisiblePieces.push_back(mGrid[i][j]);
		}
	}

	// Now sort them from distance to the camera
	SortStoredPiecesByDistance(camera);
}

// ------------------------------------------------------------------------ //

void EditorGrid::SortStoredPiecesByDistance(BaseCamera* camera)
{
	if (!camera)
		return;

	// Get the camera's position
	Vector3D cameraPos = camera->GetPosition();

	// ------------------------------------------------------------------------------------------------------------------------------- 

	// Now loop through all stored grid pieces and get their distances from the camera, storing them for later use
	std::vector<float> distances;
	Vector2D           currentPos;

	for (unsigned int i = 0; i < mVisiblePieces.size(); i++)
	{
		// Error checking, if the piece doesnt exist then something has gone wrong in the visibility checking code
		if (!mVisiblePieces[i].trackPiece)
			return;

		// Get this piece's distance
		currentPos = mVisiblePieces[i].trackPiece->GetGridPositionWorldSpace();

		distances.push_back((cameraPos - Vector3D(currentPos.x, 0.0f, currentPos.y)).LengthSquared());
	}

	// ------------------------------------------------------------------------------------------------------------------------------- 

	// Now we have the distances stored in the same order as the pieces themselves we can sort the vector by distance
	unsigned int currentLargestIndex;

	// Loop through the vector to determine which grid piece should be in this index's index
	for (unsigned int indexToSet = 0; indexToSet < mVisiblePieces.size(); indexToSet++)
	{
		currentLargestIndex = indexToSet;

		// Now loop through all of the vector, from the outer loop onwards, finding the correct distance for this index
		for (unsigned int indexChecking = indexToSet; indexChecking < mVisiblePieces.size(); indexChecking++)
		{
			// Check the current index's distance against the current largest, if it is greater than store this new index
			if (distances[currentLargestIndex] < distances[indexChecking])
			{
				currentLargestIndex = indexChecking;
			}
		}

		// Now we have completed the run through we need to swap the grid pieces around, and the distance values around
		float placeholderDistance           = distances[indexToSet];

		// Now set the correct distance to the correct position in the vector
		distances[indexToSet]               = distances[currentLargestIndex];

		// Now place the previous distance in the space we have just moved
		distances[currentLargestIndex]      = placeholderDistance;

		// Now we need to apply the change to the grid pieces vector
		GridPiece placeholderPiece          = mVisiblePieces[indexToSet];

		mVisiblePieces[indexToSet]          = mVisiblePieces[currentLargestIndex];

		mVisiblePieces[currentLargestIndex] = placeholderPiece;
	}

	// ------------------------------------------------------------------------------------------------------------------------------- 
}

// ------------------------------------------------------------------------ //