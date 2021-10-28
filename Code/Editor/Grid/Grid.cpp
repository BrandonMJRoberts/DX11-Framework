#include "Grid.h"

#include "../../Camera/BaseCamera.h"

// ------------------------------------------------------------------------ //

EditorGrid::EditorGrid(ShaderHandler& shaderHandler)
	: mOcclusionTexture(nullptr)
	, mShaderHandler(shaderHandler)
{
	// Make sure that the grid is empty
	ClearGrid();

	D3D11_TEXTURE2D_DESC desc;
	desc.Width              = 320;
	desc.Height             = 240;
	desc.MipLevels          = 1;
	desc.ArraySize          = 1;
	desc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.SampleDesc.Count   = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage              = D3D11_USAGE_DEFAULT;
	desc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags     = 0;
	desc.MiscFlags          = 0;

	// Create the texture
	mShaderHandler.CreateTexture2D(&desc, nullptr, &mOcclusionTexture);
}

// ------------------------------------------------------------------------ //

EditorGrid::~EditorGrid()
{
	ClearGrid();

	mVisibleGridPieces.clear();
}

// ------------------------------------------------------------------------ //

GridPiece EditorGrid::GetTrackPiece(unsigned int x, unsigned int y, unsigned int z)
{
	return mGrid[x][y][z];
}

// ------------------------------------------------------------------------ //

GridPiece EditorGrid::GetTrackPiece(Vector3D position)
{
	return mGrid[(unsigned int)position.x][(unsigned int)position.y][(unsigned int)position.z];
}

// ------------------------------------------------------------------------ //

void      EditorGrid::SetGridPiece(TrackPieceType typeToAdd, Vector3D position)
{
	// First check to see if the piece is not already the same thing
	if (typeToAdd == GetTrackPiece(position).pieceType)
		return;

	mGrid[(unsigned int)position.x][(unsigned int)position.y][(unsigned int)position.z].pieceType  = typeToAdd;
	mGrid[(unsigned int)position.x][(unsigned int)position.y][(unsigned int)position.z].trackPiece = TrackPieceFactory::GetInstance()->CreateTrackPiece(typeToAdd);
}

// ------------------------------------------------------------------------ //

void      EditorGrid::SetGridPiece(TrackPieceType typeToAdd, unsigned int x, unsigned int y, unsigned int z)
{
	if (typeToAdd == mGrid[x][y][z].pieceType)
		return;

	// Set the new type
	mGrid[x][y][z].pieceType  = typeToAdd;

	// Assign the track piece
	mGrid[x][y][z].trackPiece = TrackPieceFactory::GetInstance()->CreateTrackPiece(typeToAdd); 
}

// ------------------------------------------------------------------------ //

void EditorGrid::ClearGrid()
{
	for (unsigned int i = 0; i < Constants::GRID_WIDTH; i++)
	{
		for (unsigned int j = 0; j < Constants::GRID_HEIGHT; j++)
		{
			for (unsigned int k = 0; k < Constants::GRID_DEPTH; k++)
			{
				mGrid[i][j][k].pieceType  = TrackPieceType::EMPTY;
				mGrid[i][j][k].trackPiece = nullptr;
			}
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
	// Update what is now visible and now not visible

}

// ------------------------------------------------------------------------ //

void EditorGrid::Render(BaseCamera* camera)
{
	// First bind the correct texture for the draw calls
	mShaderHandler.BindTexture(mOcclusionTexture);

	// Now disable colour rendering as we just want depth and stencil 
	
	// First find what elements of the grid are visable
	// Do this by rendering to a occlusion test
	Vector3D piecePosition;
	for (unsigned int i = 0; i < mPiecesOnScreen.size(); i++)
	{
		// Render the geometry (without anything fancy) to the buffer
		mPiecesOnScreen[i].trackPiece->RenderGeometry();
	}

	// Now loop through the grid again to see what is actually visible
	for (unsigned int i = 0; i < mPiecesOnScreen.size(); i++)
	{
		// Render the geometry (without anything fancy) again, but perform an occlusion query on it

		// The test has passed so add it to the list of visible elements
		mVisibleGridPieces.push_back(mPiecesOnScreen[i]);
	}

	// Now re-bind the previous render targets from before this function
	mShaderHandler.ReBindPriorRenderTargets();

	// Now render what can be seen
	for (unsigned int i = 0; i < mVisibleGridPieces.size(); i++)
	{
		mVisibleGridPieces[i].trackPiece->RenderComplete();
	}
}

// ------------------------------------------------------------------------ //