#include "Grid.h"

#include "../../Camera/BaseCamera.h"

// ------------------------------------------------------------------------ //

EditorGrid::EditorGrid(ShaderHandler& shaderHandler)
	: mShaderHandler(shaderHandler)
{
	// Make sure that the grid is empty
	ClearGrid();

	// Make sure to initialise the factory
	TrackPieceFactory::GetInstance()->Init(&shaderHandler);

	// Create the highlight object that will change as new track types are selected
	mPotentialNewPiece.trackPiece = TrackPieceFactory::GetInstance()->CreateTrackPiece(TrackPieceType::EMPTY);
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

void EditorGrid::Render(BaseCamera* camera)
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
	// Find where the camera is currently pointing to and set the potential piece's grid position to be there
	// To do this we get the camera's position and facing direction, and find when they cross the X-Z plane
	if (!camera || !mPotentialNewPiece.trackPiece)
		return;

	/*Vector3D cameraPos = camera->GetPosition();
	Vector3D facingDir = camera->GetFacingDirection();
	float t = -cameraPos.y / facingDir.y;

	if (t < 0.0f)
		return;

	Vector2D newPos = Vector2D((int)(cameraPos.x + t * facingDir.x), 
		                       (int)(cameraPos.z + t * facingDir.z));

	mPotentialNewPiece.trackPiece->SetNewGridPosition(newPos);

	mPotentialNewPiece.trackPiece->RenderFull(camera);*/
}

// ------------------------------------------------------------------------ //