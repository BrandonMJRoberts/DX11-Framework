#include "Clouds.h"

// --------------------------------------------------------------------------------- //

VolumetricClouds::VolumetricClouds()
	: mGlobalCoverage()
	, mGlobalDensity()
{
	// Initial setup of internal data
	SetupTextures();

	SetupShaders();

	// Now generate the runtime data
	GenerateWeatherMapTexture();

	GenerateNoiseTexture();
}

// --------------------------------------------------------------------------------- //

VolumetricClouds::~VolumetricClouds()
{

}

// --------------------------------------------------------------------------------- //

void VolumetricClouds::Render()
{
	// First generate the colour/alpha texture of the clouds


	// Now combine it with the depth buffer of the normal scene to get the final render
}

// --------------------------------------------------------------------------------- //

void VolumetricClouds::Update(const float deltaTime)
{
	// Update the texture coords so the clouds can move
	mTextureCoordOffset += mMovementSpeed * deltaTime;

	mTextureCoordOffset.LoopAroundOne();
}

// --------------------------------------------------------------------------------- //

void VolumetricClouds::GenerateWeatherMapTexture()
{

}

// --------------------------------------------------------------------------------- //

void VolumetricClouds::GenerateNoiseTexture()
{

}

// --------------------------------------------------------------------------------- //

void VolumetricClouds::SetupTextures()
{

}

// --------------------------------------------------------------------------------- //

void VolumetricClouds::SetupShaders()
{

}

// --------------------------------------------------------------------------------- //