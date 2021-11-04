#ifndef _CLOUDS_H_
#define _CLOUDS_H_

#include "../Maths/CommonMaths.h"

class VolumetricClouds final
{
public:
	VolumetricClouds();
	~VolumetricClouds();

	void Render();
	void Update(const float deltaTime);

private:
	// Initialise functions
	void SetupTextures();
	void SetupShaders();

	void GenerateWeatherMapTexture();
	void GenerateNoiseTexture();

	// Variables required on the CPU
	float mGlobalCoverage; // Basic probability clouds appear
	float mGlobalDensity;  // Clouds global opacity

	// The distance that the ray will progress each step (lower runs faster but looks worse)
	float mRayMarchStepLengthLarge; // The intial steps will be larger until we hit a cloud
	float mRayMarchStepLengthSmall; // Step length inside a cloud

	float mSunRayMarchStepLength;   // The step distance for the sun lighting calculations

	Vector2D mMovementSpeed;        // Cloud movement speed
	Vector2D mTextureCoordOffset;   // The application of the movement speed

	Vector4D mAtmosphereColour;     // The colour the clouds should blend to when they are visually close to the horizon

	// Weather map - 512x512 four channel image that holds important data
		// R = Low coverage map
		// G = High coverage map
		// B = Clouds peak hat
		// A = Cloud weather-map density

	// First pass shaders (calculates the colour and opcaity of the clouds at each pixel)
		// Vertex
		// Pixel

	// Second pass shaders (combines with the depth buffer to render to the screen)
		// Vertex
		// Pixel

	// 3D noise texture - for the non-uniform density of the clouds
		// R = Low frequency Perlin-Worley noise
		// G = Medium frequency Worley noise
		// B = High Frequency Worley noise
		// A = Highest frequency Worley noise

};

#endif