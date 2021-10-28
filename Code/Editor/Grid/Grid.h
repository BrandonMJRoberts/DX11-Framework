#ifndef _GROUND_H_
#define _GROUND_H_

#include "../../Track/TrackPieceFactory.h"
#include "../../Maths/CommonMaths.h"

#include "../../Maths/Constants.h"

#include "../../Shaders/ShaderHandler.h"

// ------------------------------------------------------------------------ //

struct GridPiece
{
	GridPiece()  { pieceType = TrackPieceType::EMPTY; trackPiece = nullptr; }
	~GridPiece() { trackPiece = nullptr; }

	// The piece type 
	TrackPieceType pieceType;
	TrackPiece*    trackPiece;
};

// ------------------------------------------------------------------------ //

class BaseCamera;

class EditorGrid
{
public:
	EditorGrid(ShaderHandler& shaderHandler);
	~EditorGrid();

	GridPiece GetTrackPiece(unsigned int x, unsigned int y, unsigned int z);
	GridPiece GetTrackPiece(Vector3D position);

	void      SetGridPiece(TrackPieceType typeToAdd, Vector3D position);
	void      SetGridPiece(TrackPieceType typeToAdd, unsigned int x, unsigned int y, unsigned int z);

	void      ClearGrid();

	void      LoadInGridFromFile(std::string filePath);
	void      SaveOutGridToFile();

	void	  Render(BaseCamera* camera);
	void	  Update(const float deltaTime);

private:
	// Internal grid store
	GridPiece mGrid[Constants::GRID_WIDTH][Constants::GRID_HEIGHT][Constants::GRID_DEPTH];

	std::vector<GridPiece> mPiecesOnScreen;
	std::vector<GridPiece> mVisibleGridPieces;

	ShaderHandler&         mShaderHandler;
	ID3D11Texture2D*       mOcclusionTexture;
};

// ------------------------------------------------------------------------ //

#endif