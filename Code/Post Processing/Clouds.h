#ifndef _CLOUDS_H_
#define _CLOUDS_H_

#include "../Maths/CommonMaths.h"
#include "../Texture/Texture.h"
#include "../Shaders/ShaderHandler.h"

class VolumetricClouds final
{
public:
	VolumetricClouds(ShaderHandler& shaderHandler);
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
	Texture2D* mWeatherMap;

	// First pass shaders (calculates the colour and opcaity of the clouds at each pixel)
		// Vertex
		// Pixel
	ID3D11VertexShader* mShapeRenderVertexShader;
	ID3D11PixelShader*  mShapeRenderPixelShader;

	// Second pass shaders (combines with the depth buffer to render to the screen)
		// Vertex
		// Pixel
	ID3D11VertexShader* mFinalRenderVertexShader;
	ID3D11PixelShader*  mFinalRenderPixelShader;

	// noise texture - for the non-uniform density of the clouds
		// R = Low frequency Perlin-Worley noise
		// G = Medium frequency Worley noise
		// B = High Frequency Worley noise
		// A = Highest frequency Worley noise
	Texture2D* mShapeTexture;     // Used for making the shapes of the clouds
	Texture2D* mBlueNoiseTexture; // Used for offsetting the start step length

	ShaderHandler& mShaderHandler;
};

#endif