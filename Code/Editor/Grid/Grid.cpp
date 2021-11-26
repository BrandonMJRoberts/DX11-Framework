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

	// We need to also render the potential new placment piece based off of where the player is hovering over
	if (!camera || !mPotentialNewPiece.trackPiece)
		return;

	// Get the mouse position on the screen
	Vector2D mousePos = inputHandler.GetMousePosition();

	// First convert the mouse pos into projection space
	Vector2D normalisedDeviceCoords = Vector2D(( (2.0f * mousePos.x) / GameScreenManager::ScreenWidth) - 1, 
		                                       (-(2.0f * mousePos.y) / GameScreenManager::ScreenHeight) + 1);

	// Now convert the projection space point into view space
	DirectX::XMFLOAT4X4 projectionMat = camera->GetPerspectiveMatrix();
	normalisedDeviceCoords.x         /= projectionMat(0, 0);
	normalisedDeviceCoords.y         /= projectionMat(1, 1);

	// View space definition of the ray
	DirectX::XMVECTOR rayOriginViewSpace    = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMVECTOR rayDirectionViewSpace = DirectX::XMVectorSet(normalisedDeviceCoords.x, normalisedDeviceCoords.y, 1.0f, 0.0f);
	
	// Get the inverse view matrix from the camera
	DirectX::XMFLOAT4X4 inverseView = camera->GetInverseViewMatrix();
	
	// Using the inverse view matrix we can convert the ray into world space for the intersection tests
	rayOriginViewSpace    = DirectX::XMVector3TransformCoord(rayOriginViewSpace,     DirectX::XMLoadFloat4x4(&inverseView));
	rayDirectionViewSpace = DirectX::XMVector3TransformNormal(rayDirectionViewSpace, DirectX::XMLoadFloat4x4(&inverseView));

	// Now make the vector unit length for the calculations
	DirectX::XMFLOAT4 rayDirDirectX;
	DirectX::XMFLOAT4 rayOriginDirectX;
	DirectX::XMStoreFloat4(&rayDirDirectX,    rayDirectionViewSpace);
	DirectX::XMStoreFloat4(&rayOriginDirectX, rayOriginViewSpace);

	Vector3D rayDirection = Vector3D(rayDirDirectX.x, rayDirDirectX.y, rayDirDirectX.z);
	rayDirection.Normalise();

	Vector3D rayOrigin    = Vector3D(rayOriginDirectX.x, rayOriginDirectX.y, rayOriginDirectX.z);

	// Calculate the t factor
	float t = (-rayOrigin.y) / rayDirection.y;

	// Check that it is not behind the camera
	//if (t < 0.0f)
	//	return;

	Vector2D gridPos;

	// Set the new position
	gridPos.x = rayOrigin.x + (t * rayDirection.x);
	gridPos.y = rayOrigin.z + (t * rayDirection.z);

	if (gridPos.x > 64.0f)
		gridPos.x = 64.0f;
	else if (gridPos.x < -64.0f)
		gridPos.x = -64.0f;

	if (gridPos.y > 64.0f)
		gridPos.y = 64.0f;
	else if (gridPos.y < -64.0f)
		gridPos.y = -64.0f;

	gridPos = Vector2D(0, 0);
	mPotentialNewPiece.trackPiece->SetNewGridPosition(gridPos);

	// Bind the transparency state before rendering 

	float blendFactor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	mShaderHandler.BindBlendState(mBlendState, blendFactor);

	mPotentialNewPiece.trackPiece->RenderFull(camera);

	// Re-bind the normal blend state
	mShaderHandler.BindDefaultBlendState();
}

// ------------------------------------------------------------------------ //