#ifndef _TRACK_PIECE_FACTORY_H_
#define _TRACK_PIECE_FACTORY_H_

#include <vector>

#include "TrackPiece.h"

#include "../Shaders/ShaderHandler.h"

class Model;
class TrackCollision;

// -------------------------------------------------------------------- //

enum class TrackPieceType : unsigned int
{
	// Starts //
		START_ONE_ENTRANCE = 0,
		START_STRAIGHT_THROUGH,
		START_T_PIECE,
		START_OPEN,
		START_AIR,

	// Ends //
		END_ONE_ENTRANCE,
		END_STRAIGHT_THOUGH,
		END_T_PIECE,
		END_OPEN,
		END_AIR,	

	// Checkpoints //
		CHECKPOINT,

	// Normal Pieces //

		// Slopes
		SLOPE_UP,
		SLOPE_UP_RIGHT,
		SLOPE_UP_LEFT,

		SLOPE_DOWN,
		SLOPE_DOWN_RIGHT,
		SLOPE_DOWN_LEFT,

		// Straights + curves
		STRAIGHT_FORWARD,	
		T_PIECE,
		FOUR_CROSS,

		CURVE_RIGHT,
		CURVE_LEFT,

		// Custom
		JUMP_UP,
		JUMP_DOWN,

		GHOST,
		EMPTY,

		MAX
};

// -------------------------------------------------------------------- //

class TrackPieceFactory final
{
public:
	static TrackPieceFactory* GetInstance();

	void               SetNewShaderHandler(ShaderHandler* shaderHandler);

	void               Init(ShaderHandler* shaderHander);
	TrackPiece*        CreateTrackPiece(TrackPieceType pieceToMake, Vector2D worldPosition);

private:
	TrackPieceFactory(ShaderHandler* shaderHander);
	~TrackPieceFactory();

	static TrackPieceFactory*          mInstance;

	static ShaderHandler*              mShaderHandler;

	const std::vector<std::string>     kFilePathsToModels;
	const std::vector<std::string>     kFilePathsToCollisionData;

	std::vector<Model*>                mModels;
	std::vector<TrackCollision*>       mCollisions;

	ID3D11VertexShader* mDefaultRenderVertexShader;
	ID3D11PixelShader*  mDefaultRenderPixelShader;

	ID3D11VertexShader* mGhostRenderVertexShader;
	ID3D11PixelShader*  mGhostRenderPixelShader;
};

// -------------------------------------------------------------------- //

#endif