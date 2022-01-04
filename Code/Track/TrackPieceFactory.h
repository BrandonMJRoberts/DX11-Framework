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
		START = 0,  // Starts //
		CHECKPOINT, // Checkpoint //

		SLOPE,      

		//SLOPE_UP_RIGHT,
		//SLOPE_UP_LEFT,
		//SLOPE_DOWN_RIGHT,
		//SLOPE_DOWN_LEFT,

		STRAIGHT_FORWARD, // Straights + curves
		//T_PIECE,
		//FOUR_CROSS,

		CURVE,

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