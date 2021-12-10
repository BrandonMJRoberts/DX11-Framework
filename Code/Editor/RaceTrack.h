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

	void      Update(const float deltaTime, BaseCamera* camera);

	void      SaveTrack(std::string filePath);

	void      LoadInTrackFromFile(std::string filePath);
	void      LoadInTimes(std::ifstream& file,     std::string filePath);
	void      LoadInWeather(std::ifstream& file,   std::string filePath);
	void      LoadInTrackData(std::ifstream& file, std::string filePath);

private:
	void      SaveOutGridToFile(std::ofstream& file, std::string filePath);
	void      SaveOutBestTimesToFile(std::ofstream& file, std::string filePath);
	void      SaveOutweatherDataToFile(std::ofstream& file, std::string filePath);

	float          mBestTrackTimes[5];
	std::string    mTrackName;
	float          mTimeOfDay;
	Vector2D       mCloudMovementSpeed;
	WeatherType    mWeatherType;
	InputHandler&  mInputHandler;
	float          mGlobalCoverage;

	EditorGrid*    mRaceTrack;
	Ground*        mTrackGround;
};

// ------------------------------------------------------------------------------- //s

#endif