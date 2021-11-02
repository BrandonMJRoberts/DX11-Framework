#include "Grid.h"

#include "../../Camera/BaseCamera.h"

// ------------------------------------------------------------------------ //

EditorGrid::EditorGrid(ShaderHandler& shaderHandler)
	: mOcclusionTexture(nullptr)
	, mShaderHandler(shaderHandler)
{
	// Make sure that the grid is empty
	ClearGrid();

	// First create the texture
	D3D11_TEXTURE2D_DESC desc;
	desc.Width              = 320;
	desc.Height             = 240;
	desc.MipLevels          = 1;
	desc.ArraySize          = 1;
	desc.Format             = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count   = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage              = D3D11_USAGE_DEFAULT;
	desc.BindFlags          = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags     = 0;
	desc.MiscFlags          = 0;

	if (!mShaderHandler.CreateTexture2D(&desc, nullptr, &mOcclusionTexture) && mOcclusionTexture)
	{
		return;
	}

	// Now create the render target view through a description
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDescription;
	renderTargetViewDescription.Format             = desc.Format;
	renderTargetViewDescription.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDescription.Texture2D.MipSlice = 0;

	// This should return back the render target view into the final parameter
	mShaderHandler.CreateRenderTargetView(mOcclusionTexture, &renderTargetViewDescription, &mRenderTargetViewOcclusion);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format                    = desc.Format;
	shaderResourceViewDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels       = 1;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

	// This should return back the shader resource view into the final parameter
	mShaderHandler.CreateShaderResourceView(mOcclusionTexture, &shaderResourceViewDesc, &mShaderResourceViewOcclusion);

	// Now create the depth / stencil view
	ID3D11Texture2D* tempDepthTexture = nullptr;

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width              = 320;
	depthStencilDesc.Height				= 240;
    depthStencilDesc.MipLevels          = 1;
    depthStencilDesc.ArraySize          = 1;
    depthStencilDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count   = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage              = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags     = 0;
    depthStencilDesc.MiscFlags          = 0;

	// Create the texture
	if (!mShaderHandler.CreateTexture2D(&depthStencilDesc, nullptr, &tempDepthTexture))
		return;


	// Create the depth stencil view
	if (!mShaderHandler.CreateDepthStencilView(tempDepthTexture, nullptr, &mDepthStencilViewOcclusion))
		return;

	// Release the temp texture
	tempDepthTexture->Release();
	tempDepthTexture = nullptr;
}

// ------------------------------------------------------------------------ //

EditorGrid::~EditorGrid()
{
	ClearGrid();

	mVisibleGridPieces.clear();

	if (mOcclusionTexture)
	{
		mOcclusionTexture->Release();
		mOcclusionTexture = nullptr;
	}

	if (mRenderTargetViewOcclusion)
	{
		mRenderTargetViewOcclusion->Release();
		mRenderTargetViewOcclusion = nullptr;
	}

	if (mShaderResourceViewOcclusion)
	{
		mShaderResourceViewOcclusion->Release();
		mShaderResourceViewOcclusion = nullptr;
	}

	if (mDepthStencilViewOcclusion)
	{
		mDepthStencilViewOcclusion->Release();
		mDepthStencilViewOcclusion = nullptr;
	}
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
	mShaderHandler.SetRenderTargets(1, &mRenderTargetViewOcclusion, mDepthStencilViewOcclusion);
	//mShaderHandler.SetRenderTargets(0, NULL, mDepthStencilViewOcclusion);

	// Now clear the render target from the prior frame
	//mShaderHandler.ClearRenderTargetView(mRenderTargetViewOcclusion, Constants::COLOUR_BLACK);
	mShaderHandler.ClearDepthStencilView(mDepthStencilViewOcclusion, D3D11_CLEAR_DEPTH, 1.0f, 0);
	
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