#include "Grid.h"

#include "../../Camera/BaseCamera.h"
#include "../../GameScreens/ScreenManager.h"

// ------------------------------------------------------------------------ //

EditorGrid::EditorGrid(ShaderHandler& shaderHandler)
	: mShaderHandler(shaderHandler)
	, mBlendState(nullptr)
{
	// Make sure that the grid is empty
	ClearGrid();

	// Make sure to initialise the factory
	TrackPieceFactory::GetInstance()->Init(&shaderHandler);

	// Create the highlight object that will change as new track types are selected
	mPotentialNewPiece.trackPiece = TrackPieceFactory::GetInstance()->CreateTrackPiece(TrackPieceType::EMPTY);

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

	mGrid[(unsigned int)position.x][(unsigned int)position.y].pieceType  = typeToAdd;
	mGrid[(unsigned int)position.x][(unsigned int)position.y].trackPiece = TrackPieceFactory::GetInstance()->CreateTrackPiece(typeToAdd);
}

// ------------------------------------------------------------------------ //

void      EditorGrid::SetGridPiece(TrackPieceType typeToAdd, unsigned int x, unsigned int y)
{
	if (typeToAdd == mGrid[x][y].pieceType)
		return;

	// Set the new type
	mGrid[x][y].pieceType  = typeToAdd;

	// Assign the track piece
	mGrid[x][y].trackPiece = TrackPieceFactory::GetInstance()->CreateTrackPiece(typeToAdd); 
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

void      EditorGrid::LoadInGridFromFile(std::string filePath)
{

}

// ------------------------------------------------------------------------ //

void      EditorGrid::SaveOutGridToFile()
{

}

// ------------------------------------------------------------------------ //

void EditorGrid::Update(const float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);
}

// ------------------------------------------------------------------------ //

void EditorGrid::Render(BaseCamera* camera, InputHandler& inputHandler)
{
	for (unsigned int i = 0; i < Constants::GRID_WIDTH; i++)
	{
		for (unsigned int j = 0; j < Constants::GRID_HEIGHT; j++)
		{
			if(mGrid[i][j].trackPiece)
				mGrid[i][j].trackPiece->RenderFull(camera);
		}
	}

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
//	DirectX::XMMATRIX   projMat       = DirectX::XMLoadFloat4x4(&projectionMat);

	// Get the ray in view space
	normalisedDeviceCoords.x /= projectionMat(0, 0);
	normalisedDeviceCoords.y /= projectionMat(1, 1);

	// Define the ray origin and direction in view space
	DirectX::XMVECTOR rayOrigin = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMVECTOR rayDir    = DirectX::XMVectorSet(normalisedDeviceCoords.x, normalisedDeviceCoords.y, 1.0f, 0.0f);

	// Now convert the ray dir and origin into world space by applying the inverse view matrix

	DirectX::XMFLOAT4X4 inverseView = camera->GetInverseViewMatrix();
	DirectX::XMMATRIX   invView = DirectX::XMLoadFloat4x4(&inverseView);

	rayOrigin = DirectX::XMVector3TransformCoord(rayOrigin, invView);
	rayDir    = DirectX::XMVector3TransformNormal(rayDir, invView);

	// Normalise the direction vector
	rayDir = DirectX::XMVector3Normalize(rayDir);

	// Convert to useful variables
	DirectX::XMFLOAT4 origin, direction;
	DirectX::XMStoreFloat4(&origin, rayOrigin);
	DirectX::XMStoreFloat4(&direction, rayDir);

	// Now we have the ray origin and direction in world space, so check for the intersection with the ground


	// Calculate the time factor
	float t = (-origin.y) / direction.y;

	// Check that it is not behind the camera
	//if (t < 0.0f)
	//	return;

	Vector2D gridPos;

	// Set the new position
	gridPos.x = origin.x + (t * direction.x);
	gridPos.y = origin.z + (t * direction.z);

	if (gridPos.x > 64.0f)
		gridPos.x = 64.0f;
	else if (gridPos.x < -64.0f)
		gridPos.x = -64.0f;

	if (gridPos.y > 64.0f)
		gridPos.y = 64.0f;
	else if (gridPos.y < -64.0f)
		gridPos.y = -64.0f;

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