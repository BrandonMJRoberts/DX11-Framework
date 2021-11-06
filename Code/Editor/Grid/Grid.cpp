#include "Grid.h"

#include "../../Camera/BaseCamera.h"

// ------------------------------------------------------------------------ //

EditorGrid::EditorGrid(ShaderHandler& shaderHandler)
	: mOcclusionRenderBuffer(nullptr)
	, mShaderHandler(shaderHandler)
	, mDepthStencilBufferOcclusion(nullptr)
{
	// Make sure that the grid is empty
	ClearGrid();

	mOcclusionRenderBuffer = new RenderBuffer(shaderHandler, 320, 240, 1, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_USAGE_DEFAULT);

	mDepthStencilBufferOcclusion = new DepthStencilBuffer(shaderHandler, 320, 240, 1, 1, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_USAGE_DEFAULT, DXGI_FORMAT_D24_UNORM_S8_UINT, false);
}

// ------------------------------------------------------------------------ //

EditorGrid::~EditorGrid()
{
	ClearGrid();

	mVisibleGridPieces.clear();

	delete mOcclusionRenderBuffer;
	mOcclusionRenderBuffer = nullptr;

	delete mDepthStencilBufferOcclusion;
	mDepthStencilBufferOcclusion = nullptr;
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
	UNREFERENCED_PARAMETER(deltaTime);

	// Update what is now visible and now not visible

}

// ------------------------------------------------------------------------ //

void EditorGrid::Render(BaseCamera* camera)
{
	// First bind the correct texture for the draw calls
	if (mOcclusionRenderBuffer && mDepthStencilBufferOcclusion)
	{
		mOcclusionRenderBuffer->BindRenderTargetAsActive(mDepthStencilBufferOcclusion->GetDepthStencilView());
		mOcclusionRenderBuffer->ClearRenderBuffer(mDepthStencilBufferOcclusion->GetDepthStencilView());
	}
	else 
		return;
	
	// First find what elements of the grid are visible
	// Do this by rendering to a occlusion test
	Vector3D piecePosition;
	for (unsigned int i = 0; i < mPiecesOnScreen.size(); i++)
	{
		// Render the geometry (without anything fancy) to the occlusion buffer
		//mPiecesOnScreen[i].trackPiece->RenderGeometry();
	}

	// Now loop through the grid again to see what is actually visible
	for (unsigned int i = 0; i < mPiecesOnScreen.size(); i++)
	{
		// Render the geometry (without anything fancy) again, but perform an occlusion query on it now that everything has been drawn before

		// The test has passed so add it to the list of visible elements
		mVisibleGridPieces.push_back(mPiecesOnScreen[i]);
	}

	// Now re-bind the previous render targets from before this function
	mShaderHandler.SetPriorRenderTarget();

	// Now render what can be seen to the actual back buffer
	for (unsigned int i = 0; i < mVisibleGridPieces.size(); i++)
	{
		// Now render the geometry normally to the default back buffer
		//mVisibleGridPieces[i].trackPiece->RenderComplete();
	}
}

// ------------------------------------------------------------------------ //