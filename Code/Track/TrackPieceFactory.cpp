#include "TrackPieceFactory.h"

#include "../Models/Model.h"
#include "../Collisions/TrackCollision.h"

// -------------------------------------------------------------------- //

TrackPieceFactory::TrackPieceFactory(ShaderHandler& shaderHander) 
	: mShaderHandler(shaderHander)
	, kFilePathsToModels({ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", })
	, kFilePathsToCollisionData({ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", })
{
	// Load in the models for the track pieces
	for (unsigned int i = 0; i < (unsigned int)TrackPieceType::MAX; i++)
	{
		if(kFilePathsToModels.size() > i)
			mModels.push_back(new Model(mShaderHandler, kFilePathsToModels[i]));
	}

	// Load in the collision data for the track pieces
	for (unsigned int i = 0; i < (unsigned int)TrackPieceType::MAX; i++)
	{
		if (kFilePathsToCollisionData.size() > i)
			mCollisions.push_back(new TrackCollision(kFilePathsToCollisionData[i]));
	}
}

// -------------------------------------------------------------------- //

TrackPieceFactory::~TrackPieceFactory()
{
	mModels.clear();
	mCollisions.clear();
}

// -------------------------------------------------------------------- //

TrackPiece* TrackPieceFactory::CreateTrackPiece(TrackPieceType pieceType)
{
	// Error checking just incase we are de-referencing a nullptr
	if (!mModels[(unsigned int)pieceType] || !mCollisions[(unsigned int)pieceType])
		return nullptr;

	// Create the piece with the correct data
	TrackPiece* returnTrackPiece = new TrackPiece(*(mModels[(unsigned int)pieceType]), *(mCollisions[(unsigned int)pieceType]));

	// If not a created for some reason then return nullptr
	return returnTrackPiece;
}

// -------------------------------------------------------------------- //