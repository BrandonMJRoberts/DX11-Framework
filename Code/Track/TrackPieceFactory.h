#ifndef _TRACK_PIECE_FACTORY_H_
#define _TRACK_PIECE_FACTORY_H_

#include "TrackPiece.h"

// -------------------------------------------------------------------- //

enum class TrackPieceType
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
		CHECKPOINT_STRAIGHT_TRACK,
		CHECKPOINT_AIR,
		CHECKPOINT_CURVE_LEFT,
		CHECKPOINT_CURVE_RIGHT,
		CHECKPOINT_SLOPE_UP,
		CHECKPOINT_SLOPE_DOWN,


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
		JUMP_DOWN

};

// -------------------------------------------------------------------- //

static class TrackPieceFactory final
{
public:
	void Init();
	void Destroy();

	TrackPiece* CreateTrackPiece(TrackPieceType pieceToMake);

private:
	TrackPieceFactory();
	~TrackPieceFactory();

	// std::vector<Models*> mModels;
	// std::vector<Collisions*> mCollisions;
};

// -------------------------------------------------------------------- //

#endif