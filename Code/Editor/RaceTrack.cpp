#include "RaceTrack.h"

#include "Grid/Grid.h"
#include "Ground/Ground.h"
#include "../Shaders/ShaderHandler.h"
#include "../Input/InputHandler.h"

#include <fstream>

// -------------------------------------------------------------------------------- //

RaceTrack::RaceTrack(ShaderHandler& shaderHandler, InputHandler& inputHandler, std::string trackName)
	: mBestTrackTimes{30.0f, 40.0f, 50.0f, 60.0f, 70.0f}
	, mTrackName("FillerName")
	, mTimeOfDay(1200)
	, mCloudMovementSpeed(0.0f, 0.0f)
	, mWeatherType(WeatherType::SUN)
	, mRaceTrack(nullptr)
	, mTrackGround(nullptr)
	, mInputHandler(inputHandler)
{
	mRaceTrack   = new EditorGrid(shaderHandler);
	mTrackGround = new Ground(shaderHandler);
}

// -------------------------------------------------------------------------------- //

RaceTrack::~RaceTrack()
{
	delete mTrackGround;
	mTrackGround = nullptr;

	delete mRaceTrack;
	mRaceTrack = nullptr;
}

// -------------------------------------------------------------------------------- //

void RaceTrack::RenderGround(BaseCamera* camera)
{
	if (mTrackGround)
	{
		mTrackGround->Render(camera);
	}
}

// -------------------------------------------------------------------------------- //

void RaceTrack::RenderGrid(BaseCamera* camera)
{
	if (mRaceTrack)
	{
		mRaceTrack->Render(camera, mInputHandler);
	}
}

// -------------------------------------------------------------------------------- //

void RaceTrack::Update(const float deltaTime)
{
	if (mTrackGround)
	{
		mTrackGround->Update(deltaTime);
	}

	if (mRaceTrack)
	{
		mRaceTrack->Update(deltaTime);
	}
}

// -------------------------------------------------------------------------------- //

void RaceTrack::SaveTrack(std::string filePath)
{
	// First create a file in the correct place
	std::ofstream file;
	file.open(filePath);

	if (file.is_open() && file.good())
	{
		// First write out the xml format header
		std::string outLine = "<?xml version=";
		outLine += '"' + std::to_string(1.0) + '"';
		outLine += " encoding=" + '"';
		outLine += "UTF-8" + '"';
		outLine += "?>";
		file.write(outLine.c_str(), outLine.length());

		// Now open the first grouping of brackets
		outLine = "<TrackGrid>";
		file.write(outLine.c_str(), outLine.length());

		// Now go through each section of the track and output the data
		// First make sure to close the file as we are going to have to re-open it
		file.close();

		SaveOutBestTimesToFile(file);

		SaveOutweatherDataToFile(file);

		SaveOutGridToFile(file);

		// Now re-open the file
		file.open(filePath);
		if (!file.is_open() || !file.good())
		{
			return;
		}

		// Close the backets grouping
		outLine = "</TrackGrid>";
		file.write(outLine.c_str(), outLine.length());
	}

	file.close();
}

// -------------------------------------------------------------------------------- //

void RaceTrack::SaveOutGridToFile(std::ofstream& file)
{
	
}

// -------------------------------------------------------------------------------- //

void RaceTrack::SaveOutBestTimesToFile(std::ofstream& file)
{

}

// -------------------------------------------------------------------------------- //

void RaceTrack::SaveOutweatherDataToFile(std::ofstream& file)
{

}

// -------------------------------------------------------------------------------- //

void RaceTrack::LoadInTrackFromFile(std::string filePath) 
{

}

// -------------------------------------------------------------------------------- //