#ifndef _GROUND_H_
#define _GROUND_H_

#include "../../Track/TrackPieceFactory.h"
#include "../../Maths/CommonMaths.h"

#include "../../Maths/Constants.h"

#include "../../Shaders/ShaderHandler.h"

#include "../../Texture/RenderBuffer.h"

#include <string>

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
	EditorGrid(ShaderHandler& shaderHandler, std::string filePathForTrackLoad);
	~EditorGrid();

	GridPiece GetTrackPiece(unsigned int x, unsigned int y);
	GridPiece GetTrackPiece(Vector2D position);

	void      SetGridPiece(TrackPieceType typeToAdd, Vector2D position);
	void      SetGridPiece(TrackPieceType typeToAdd, unsigned int x, unsigned int y);

	void      ClearGrid();

	void      LoadInGridFromFile(std::string filePath);

	void      SaveOutTrackInternalDataToFile();


	void	  Render(BaseCamera* camera, InputHandler& inputHandler);
	void	  Update(const float deltaTime, InputHandler& inputHandler, BaseCamera* camera);

	void      RefreshGridVisibility(BaseCamera* camera);

private:
	void      FindAllVisibleGridPieces(BaseCamera* camera);
	void      SortStoredPiecesByDistance(BaseCamera* camera);

	// Internal grid store
	GridPiece      mGrid[Constants::GRID_WIDTH][Constants::GRID_HEIGHT];
	GridPiece      mPotentialNewPiece;

	std::vector<GridPiece> mVisiblePieces;

	ID3D11BlendState* mBlendState;

	ShaderHandler&    mShaderHandler;

	bool              mAddPieceState;
	bool              mSwappedState;
};

// ------------------------------------------------------------------------ //

#endif