#include "TrackPieceFactory.h"

// -------------------------------------------------------------------- //

TrackPieceFactory::TrackPieceFactory()
{

}

// -------------------------------------------------------------------- //

TrackPieceFactory::~TrackPieceFactory()
{

}

// -------------------------------------------------------------------- //

void TrackPieceFactory::Init()
{

}

// -------------------------------------------------------------------- //

void TrackPieceFactory::Destroy()
{

}

// -------------------------------------------------------------------- //

TrackPiece* TrackPieceFactory::CreateTrackPiece(TrackPieceType pieceType)
{
	// Create the correct piece type
	switch (pieceType)
	{

	// Starts //
	case TrackPieceType::START_ONE_ENTRANCE:
	break;

	case TrackPieceType::START_STRAIGHT_THROUGH:
	break;

	case TrackPieceType::START_T_PIECE:
	break;

	case TrackPieceType::START_OPEN:	
	break;

	case TrackPieceType::START_AIR:
	break;

	// Ends //
	case TrackPieceType::END_ONE_ENTRANCE:
	break;

	case TrackPieceType::END_STRAIGHT_THOUGH:
	break;

	case TrackPieceType::END_T_PIECE:
	break;

	case TrackPieceType::END_OPEN:
	break;

	case TrackPieceType::END_AIR:
	break;

	// Checkpoints //
	case TrackPieceType::CHECKPOINT_STRAIGHT_TRACK:
	break;

	case TrackPieceType::CHECKPOINT_AIR:
	break;

	case TrackPieceType::CHECKPOINT_CURVE_LEFT:
	break;

	case TrackPieceType::CHECKPOINT_CURVE_RIGHT:
	break;

	case TrackPieceType::CHECKPOINT_SLOPE_UP:
	break;

	case TrackPieceType::CHECKPOINT_SLOPE_DOWN:
	break;

	// Slopes //
	case TrackPieceType::SLOPE_UP:
	break;

	case TrackPieceType::SLOPE_UP_RIGHT:
	break;

	case TrackPieceType::SLOPE_UP_LEFT:
	break;

	case TrackPieceType::SLOPE_DOWN:
	break;

	case TrackPieceType::SLOPE_DOWN_RIGHT:
	break;

	case TrackPieceType::SLOPE_DOWN_LEFT:
	break;

	// Normal pieces //
	case TrackPieceType::STRAIGHT_FORWARD:
	break;

	case TrackPieceType::T_PIECE:
	break;

	case TrackPieceType::FOUR_CROSS:
	break;

	case TrackPieceType::CURVE_RIGHT:
	break;

	case TrackPieceType::CURVE_LEFT:
	break;

	case TrackPieceType::JUMP_UP:
	break;

	case TrackPieceType::JUMP_DOWN:
	break;

	default:break;
	}

	// If not a created for some reason then return nullptr
	return nullptr;
}

// -------------------------------------------------------------------- //