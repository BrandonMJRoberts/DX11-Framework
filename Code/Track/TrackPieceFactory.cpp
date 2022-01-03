#include "TrackPieceFactory.h"

#include "../Models/Model.h"
#include "../Collisions/TrackCollision.h"

TrackPieceFactory* TrackPieceFactory::mInstance      = nullptr;
ShaderHandler*     TrackPieceFactory::mShaderHandler = nullptr;

// -------------------------------------------------------------------- //

TrackPieceFactory::TrackPieceFactory(ShaderHandler* shaderHander) 
	: kFilePathsToModels({ "Models/Track/Finish/Finish.obj", 
		                   "Models/Track/Checkpoint/Checkpoint.obj", 
		                   "Models/Track/Jump/Jump.obj", 
		                   "Models/Track/Empty/PlaceholderGhost.obj", 
		                   "Models/Track/Empty/PlaceholderGhost.obj", 
		                   "Models/Track/Empty/PlaceholderGhost.obj",
		                   "Models/Track/Empty/PlaceholderGhost.obj",  
		                   "Models/Track/Straight/StraightTrack.obj", 
		                   "Models/Track/Empty/PlaceholderGhost.obj", 
		                   "Models/Track/Empty/PlaceholderGhost.obj", 
		                   "Models/Track/Empty/PlaceholderGhost.obj", 
		                   "Models/Track/Empty/PlaceholderGhost.obj", 
		                   "Models/Track/Empty/PlaceholderGhost.obj" })
	, kFilePathsToCollisionData({ "", "", "", "", "", "", "", "", "", "", "", "", "",})
	, mDefaultRenderVertexShader(nullptr)
	, mDefaultRenderPixelShader(nullptr)
	, mGhostRenderVertexShader(nullptr)
	, mGhostRenderPixelShader(nullptr)
{
	UNREFERENCED_PARAMETER(shaderHander);
}

// -------------------------------------------------------------------- //

TrackPieceFactory::~TrackPieceFactory()
{
	mModels.clear();
	mCollisions.clear();

	if (mDefaultRenderVertexShader)
	{
		mDefaultRenderVertexShader->Release();
		mDefaultRenderVertexShader = nullptr;
	}

	if (mDefaultRenderPixelShader)
	{
		mDefaultRenderPixelShader->Release();
		mDefaultRenderPixelShader = nullptr;
	}

	if (mGhostRenderVertexShader)
	{
		mGhostRenderVertexShader->Release();
		mGhostRenderVertexShader = nullptr;
	}

	if (mGhostRenderPixelShader)
	{
		mGhostRenderPixelShader->Release();
		mGhostRenderPixelShader = nullptr;
	}
}

// -------------------------------------------------------------------- //

TrackPiece* TrackPieceFactory::CreateTrackPiece(TrackPieceType pieceType, Vector2D worldPosition)
{
	// Error checking just in case we are de-referencing a nullptr
	if (mModels.size() < (unsigned int)pieceType || !mModels[(unsigned int)pieceType])// || !mCollisions[(unsigned int)pieceType])
		return nullptr;

	// Create the piece with the correct data
	TrackPiece* returnTrackPiece = new TrackPiece(*(mModels[(unsigned int)pieceType]), worldPosition); //, *(mCollisions[(unsigned int)pieceType]));

	// If not a created for some reason then return nullptr
	return returnTrackPiece;
}

// -------------------------------------------------------------------- //

void TrackPieceFactory::Init(ShaderHandler* shaderHander)
{
	if (!shaderHander)
		return;

	// Set the shader handler
	mShaderHandler = shaderHander;

	// Load in the shaders
	VertexShaderReturnData returnDataDefault = mShaderHandler->CompileVertexShader(L"ModelLightingRender.fx", "VS");
	VertexShaderReturnData returnDataGhostly = mShaderHandler->CompileVertexShader(L"ModelGhostlyRender.fx", "VS");

	mDefaultRenderVertexShader = returnDataDefault.vertexShader;
	mGhostRenderVertexShader   = returnDataGhostly.vertexShader;

	// Pixel shaders
	mDefaultRenderPixelShader = mShaderHandler->CompilePixelShader(L"ModelLightingRender.fx", "PS");
	mGhostRenderPixelShader   = mShaderHandler->CompilePixelShader(L"ModelGhostlyRender.fx", "PS");

	// Load in the models for the track pieces
	for (unsigned int i = 0; i < (unsigned int)TrackPieceType::MAX; i++)
	{
		if (kFilePathsToModels.size() > i && kFilePathsToModels[i] != "")
		{
			// Pass through the ghostly version of the shader
			if ((TrackPieceType)i == TrackPieceType::GHOST)
			{
				mModels.push_back(new Model(*mShaderHandler, kFilePathsToModels[i], mGhostRenderVertexShader, mGhostRenderPixelShader, returnDataGhostly.Blob, false));
			}
			else
			{
				// Otherwise just pass through the normal render of the shader
				mModels.push_back(new Model(*mShaderHandler, kFilePathsToModels[i], mDefaultRenderVertexShader, mDefaultRenderPixelShader, returnDataDefault.Blob, false));
			}
		}
	}

	if(returnDataDefault.Blob)
		returnDataDefault.Blob->Release();

	if(returnDataGhostly.Blob)
		returnDataGhostly.Blob->Release();

	// Load in the collision data for the track pieces
	//for (unsigned int i = 0; i < (unsigned int)TrackPieceType::MAX; i++)
	//{
		//if (kFilePathsToCollisionData.size() > i)
		//	mCollisions.push_back(new TrackCollision(kFilePathsToCollisionData[i]));
	//}
}

// -------------------------------------------------------------------- //

TrackPieceFactory* TrackPieceFactory::GetInstance()
{
	if (!mInstance)
	{
		mInstance = new TrackPieceFactory(mShaderHandler);
	}

	return mInstance;
}

// -------------------------------------------------------------------- //

void TrackPieceFactory::SetNewShaderHandler(ShaderHandler* shaderHandler)
{
	mShaderHandler = shaderHandler;
}

// -------------------------------------------------------------------- //