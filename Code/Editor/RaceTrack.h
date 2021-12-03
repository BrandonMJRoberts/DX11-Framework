#ifndef _RACE_TRACK_H_
#define _RACE_TRACK_H_

#include "../Maths/CommonMaths.h"

#include <string>

class EditorGrid;
class Ground;
class ShaderHandler;
class InputHandler;
class BaseCamera;

// ------------------------------------------------------------------------------- //

enum class WeatherType
{
	SUN,
	RAIN, 
	SNOW,
	ICE,

	MAX
};

// ------------------------------------------------------------------------------- //

class RaceTrack final
{
public:
	RaceTrack(ShaderHandler& shaderHandler, InputHandler& inputHandler, std::string trackName);
	~RaceTrack();

	void      RenderGround(BaseCamera* camera);
	void      RenderGrid(BaseCamera* camera);

	void      Update(const float deltaTime);

	void      SaveTrack(std::string filePath);

	void      LoadInTrackFromFile(std::string filePath);

private:
	void      SaveOutGridToFile(std::ofstream& file);
	void      SaveOutBestTimesToFile(std::ofstream& file);
	void      SaveOutweatherDataToFile(std::ofstream& file);

	float          mBestTrackTimes[5];
	std::string    mTrackName;
	float          mTimeOfDay;
	Vector2D       mCloudMovementSpeed;
	WeatherType    mWeatherType;
	InputHandler&  mInputHandler;

	EditorGrid*    mRaceTrack;
	Ground*        mTrackGround;
};

// ------------------------------------------------------------------------------- //s

#endif