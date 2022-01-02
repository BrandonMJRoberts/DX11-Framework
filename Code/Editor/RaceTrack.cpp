#include "RaceTrack.h"

#include "Grid/Grid.h"
#include "Ground/Ground.h"
#include "../Shaders/ShaderHandler.h"
#include "../Input/InputHandler.h"

#include <fstream>
#include <sstream>

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
	, mGlobalCoverage(0.5f)
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

void RaceTrack::Update(const float deltaTime, BaseCamera* camera)
{
	if (mTrackGround)
	{
		mTrackGround->Update(deltaTime);
	}

	if (mRaceTrack)
	{
		mRaceTrack->Update(deltaTime, mInputHandler, camera);
	}

	if (mInputHandler.GetIsKeyPressed('M'))
	{
		SaveTrack("Saved Tracks/ComputerGeneratedExample.xml");
	}
	else if (mInputHandler.GetIsKeyPressed('L'))
	{
		LoadInTrackFromFile("Saved Tracks/ComputerGeneratedExample.xml");

		mRaceTrack->RefreshGridVisibility(camera);
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
		std::string outLine = "<?xml version=\"" + std::to_string(1.0) + "\" encoding=\"UTF-8\"?>\n";
		file.write(outLine.c_str(), outLine.length());

		// Now open the first grouping of brackets
		outLine = "\n<TrackGrid>\n";
		file.write(outLine.c_str(), outLine.length());

		// Now go through each section of the track and output the data
		// First make sure to close the file as we are going to have to re-open it
		file.close();

		SaveOutBestTimesToFile(file, filePath);

		SaveOutweatherDataToFile(file, filePath);

		SaveOutGridToFile(file, filePath);

		SaveOutGridRotationToFile(file, filePath);

		// Now re-open the file
		file.open(filePath, std::ios::app);
		if (!file.is_open() || !file.good())
		{
			return;
		}

		// Close the backets grouping
		outLine = "\n</TrackGrid>\n";
		file.write(outLine.c_str(), outLine.length());
	}

	file.close();
}

// -------------------------------------------------------------------------------- //

void RaceTrack::SaveOutGridToFile(std::ofstream& file, std::string filePath)
{
	// First re-open the file
	file.open(filePath.c_str(), std::ios::app);

	if (file.is_open() && file.good())
	{
		// Now write the data to the file
		std::string lineToWrite = "\n\t\t<!-- The dimensions are always the same so no need to store the specific dimensions -->\n";
		file.write(lineToWrite.c_str(), lineToWrite.length());

		lineToWrite = "\t\t<!-- These files are usally computer generated so writing layers is not a normal probelm with this approach-->\n";
		file.write(lineToWrite.c_str(), lineToWrite.length());

		lineToWrite = "\t\t<!-- The ID's stored here refer to an interal enum of values in the grid class -->\n";
		file.write(lineToWrite.c_str(), lineToWrite.length());

		lineToWrite = "\t<TrackData>\n";
		file.write(lineToWrite.c_str(), lineToWrite.length());

			// Now write out the internal store of the track
			for (unsigned int i = 0; i < 16; i++)
			{
				lineToWrite = "\t\t";

				for (unsigned int j = 0; j < 16; j++)
				{
					lineToWrite.append(std::to_string((int)(mRaceTrack->GetTrackPiece(i, j).pieceType)) + " ");
				}

				lineToWrite.append("\n");
				file.write(lineToWrite.c_str(), lineToWrite.length());
			}

		lineToWrite = "\t</TrackData>\n";
		file.write(lineToWrite.c_str(), lineToWrite.length());
	}

	file.close();
}

// -------------------------------------------------------------------------------- //

void RaceTrack::SaveOutBestTimesToFile(std::ofstream& file, std::string filePath)
{
	// First re-open the file
	file.open(filePath.c_str(), std::ios::app);

	if (file.is_open() && file.good())
	{
		// Now write the data to the file
		std::string lineToWrite = "\n\t<!-- Times stored in seconds -->";
		file.write(lineToWrite.c_str(), lineToWrite.length());

		lineToWrite = "\n\t<Times>\n";
		file.write(lineToWrite.c_str(), lineToWrite.length());

			lineToWrite = "\t\t<First> " + std::to_string(mBestTrackTimes[0]) + " </First>\n";
			file.write(lineToWrite.c_str(), lineToWrite.length());

			lineToWrite = "\t\t<Second> " + std::to_string(mBestTrackTimes[1]) + " </Second>\n";
			file.write(lineToWrite.c_str(), lineToWrite.length());

			lineToWrite = "\t\t<Third> " + std::to_string(mBestTrackTimes[2]) + " </Third>\n";
			file.write(lineToWrite.c_str(), lineToWrite.length());

			lineToWrite = "\t\t<Fourth> " + std::to_string(mBestTrackTimes[3]) + " </Fourth>\n";
			file.write(lineToWrite.c_str(), lineToWrite.length());

			lineToWrite = "\t\t<Fifth> " + std::to_string(mBestTrackTimes[4]) + " </Fifth>\n";
			file.write(lineToWrite.c_str(), lineToWrite.length());

		lineToWrite = "\t</Times>\n";
		file.write(lineToWrite.c_str(), lineToWrite.length());
	}

	file.close();
}

// -------------------------------------------------------------------------------- //

void RaceTrack::SaveOutweatherDataToFile(std::ofstream& file, std::string filePath)
{
	// First re-open the file
	file.open(filePath.c_str(), std::ios::app);

	if (file.is_open() && file.good())
	{
		// Now write the data to the file
		std::string lineToWrite = "\n\t<!-- Weather data for this track design-->\n";
		file.write(lineToWrite.c_str(), lineToWrite.length());

		lineToWrite = "\t<Weather>\n\n";
		file.write(lineToWrite.c_str(), lineToWrite.length());

			lineToWrite = "\t\t<!-- Time of day stored as a 24 hour time -->\n";
			file.write(lineToWrite.c_str(), lineToWrite.length());

			lineToWrite = "\t\t<TimeOfDay> " + std::to_string((int)mTimeOfDay) + " </TimeOfDay>\n";
			file.write(lineToWrite.c_str(), lineToWrite.length());

			lineToWrite = "\n\t\t<Clouds>\n";
			file.write(lineToWrite.c_str(), lineToWrite.length());
		
				lineToWrite = "\t\t\t<MovementSpeedX> " + std::to_string(mCloudMovementSpeed.x) + " </MovementSpeedX>\n";
				file.write(lineToWrite.c_str(), lineToWrite.length());

				lineToWrite = "\t\t\t<MovementSpeedY> " + std::to_string(mCloudMovementSpeed.y) + " </MovementSpeedY>\n";
				file.write(lineToWrite.c_str(), lineToWrite.length());

				lineToWrite = "\n\t\t\t<GlobalCoverage> " + std::to_string(mGlobalCoverage) + " </GlobalCoverage>\n";
				file.write(lineToWrite.c_str(), lineToWrite.length());

			lineToWrite = "\t\t</Clouds>\n";
			file.write(lineToWrite.c_str(), lineToWrite.length());

			lineToWrite = "\n\t\t<!-- Can be 'Sun', 'Rain', 'Snow', or 'Fog' -->\n";
			file.write(lineToWrite.c_str(), lineToWrite.length());

			lineToWrite = "\t\t<Type>";

			switch (mWeatherType)
			{
			case WeatherType::SUN:
				lineToWrite.append(" SUN ");
			break;

			case WeatherType::ICE:
				lineToWrite.append(" ICE ");
			break;

			case WeatherType::RAIN:
				lineToWrite.append(" RAIN ");
			break;

			case WeatherType::SNOW:
				lineToWrite.append(" SNOW ");
			break;
			}

			lineToWrite.append("</Type>\n");
			file.write(lineToWrite.c_str(), lineToWrite.length());

		lineToWrite = "\t</Weather>\n";
		file.write(lineToWrite.c_str(), lineToWrite.length());
	}

	file.close();
}

// -------------------------------------------------------------------------------- //

void RaceTrack::SaveOutGridRotationToFile(std::ofstream& file, std::string filePath)
{
	// First re-open the file
	file.open(filePath.c_str(), std::ios::app);

	if (file.is_open() && file.good())
	{
		// Now write the data to the file
		std::string lineToWrite = "\n\t\t<!-- The dimensions are the same as above so no need to store -->\n";
		file.write(lineToWrite.c_str(), lineToWrite.length());

		lineToWrite = "\t\t<!-- The ID's stored here are: 0 - No rotation, 1 - 90 degrees rotation and so on-->\n";
		file.write(lineToWrite.c_str(), lineToWrite.length());

		lineToWrite = "\t<RotationData>\n";
		file.write(lineToWrite.c_str(), lineToWrite.length());

			// Now write out the internal store of the track
			for (unsigned int i = 0; i < 16; i++)
			{
				lineToWrite = "\t\t";

				for (unsigned int j = 0; j < 16; j++)
				{
					if(mRaceTrack->GetTrackPiece(i, j).trackPiece)
						lineToWrite.append(std::to_string(mRaceTrack->GetTrackPiece(i, j).trackPiece->GetRotationAmount()) + " ");
					else
						lineToWrite.append("0 ");
				}

				lineToWrite.append("\n");
				file.write(lineToWrite.c_str(), lineToWrite.length());
			}

		lineToWrite = "\t</RotationData>\n";
		file.write(lineToWrite.c_str(), lineToWrite.length());
	}

	file.close();
}

// -------------------------------------------------------------------------------- //

void RaceTrack::LoadInTrackFromFile(std::string filePath)
{
	// Open the file
	std::ifstream file;
	file.open(filePath.c_str(), std::ios::in);

	// Error checking
	if (file.is_open() && file.good())
	{
		// Clear the prior grid
		mRaceTrack->ClearGrid();

		// Check to see if the format of the file is correct
		std::string line;
		std::getline(file, line);

		// Check if the file is of the correct version
		if (line != "<?xml version=\"" + std::to_string(1.0) + "\" encoding=\"UTF-8\"?>")
		{
			file.close();
			return;
		}

		std::stringstream ssline;
		std::string       placeholder = "";
		std::streampos    currentFilePos;

		// Now load in the new data
		while (std::getline(file, line))
		{
			placeholder =  "";
			ssline      =  std::stringstream(line);
			ssline      >> placeholder;

			if (placeholder == "</TrackData>")
			{
				file.close();
				return;
			}

			// Check to see what we are handling now
			if (placeholder == "<Times>")
			{
				// Get the current position in the file
				currentFilePos = file.tellg();

				file.close();
				currentFilePos = LoadInTimes(file, filePath, currentFilePos);

				// Now re-open the file
				file.open(filePath.c_str(), std::ios::in);

				file.seekg(currentFilePos);
			}
			 
			if (placeholder == "<Weather>")
			{
				// Get the current position in the file
				currentFilePos = file.tellg();

				file.close();
				currentFilePos = LoadInWeather(file, filePath, currentFilePos);

				// Now re-open the file
				file.open(filePath.c_str(), std::ios::in);

				file.seekg(currentFilePos);
			}

			if (placeholder == "<TrackData>")
			{
				// Get the current position in the file
				currentFilePos = file.tellg();

				file.close();
				currentFilePos = LoadInTrackData(file, filePath, currentFilePos);

				// Now re-open the file
				file.open(filePath.c_str(), std::ios::in);

				file.seekg(currentFilePos);
			}
		}
	}
}

// -------------------------------------------------------------------------------- //

std::streampos RaceTrack::LoadInTimes(std::ifstream& file, std::string filePath, std::streampos currentFilePos)
{
	// First re-open the file
	file.open(filePath.c_str(), std::ios::in);

	if (file.is_open() && file.good())
	{
		// Now read in the data from the file, checking to see what data we are loading in
		std::string       line;
		std::string       placeHolder = "";
		std::stringstream ssLine;
		unsigned int      timesRead = 0;
		float             currentTimeBeingRead = 0.0f;

		// Go back to where the prior function left off
		file.seekg(currentFilePos);

		while (std::getline(file, line))
		{
			// First extract the time name we are dealing with
			placeHolder =  "";
			ssLine      =  std::stringstream(line);
			ssLine      >> placeHolder;

			if (placeHolder == "<First>")
			{
				ssLine >> currentTimeBeingRead;

				mBestTrackTimes[0] = currentTimeBeingRead;

				timesRead++;
			}
			else if (placeHolder == "<Second>")
			{
				ssLine >> currentTimeBeingRead;

				mBestTrackTimes[1] = currentTimeBeingRead;

				timesRead++;
			}
			else if (placeHolder == "<Third>")
			{
				ssLine >> currentTimeBeingRead;

				mBestTrackTimes[2] = currentTimeBeingRead;

				timesRead++;
			}
			else if (placeHolder == "<Fourth>")
			{
				ssLine >> currentTimeBeingRead;

				mBestTrackTimes[3] = currentTimeBeingRead;

				timesRead++;
			}
			else if (placeHolder == "<Fifth>")
			{
				ssLine >> currentTimeBeingRead;

				mBestTrackTimes[4] = currentTimeBeingRead;

				timesRead++;
			}

			if (timesRead == 5)
				break;
		}
	}

	currentFilePos = file.tellg();

	file.close();

	return currentFilePos;
}

// -------------------------------------------------------------------------------- //

std::streampos RaceTrack::LoadInWeather(std::ifstream& file, std::string filePath, std::streampos currentFilePos)
{
	// First re-open the file
	file.open(filePath.c_str(), std::ios::in);

	std::string       line;
	std::string       placeholder = "";
	std::stringstream ssLine;

	if (file.is_open() && file.good())
	{
		// Go back to where the prior function left off
		file.seekg(currentFilePos);

		// Read in the lines from the file
		while (std::getline(file, line))
		{
			placeholder =  "";
			ssLine      =  std::stringstream(line);
			ssLine      >> placeholder;

			// Handle time of day
			if (placeholder == "<TimeOfDay>")
			{
				// Extract the time of day for this track
				ssLine >> mTimeOfDay;

				continue;
			}

			// Handle clouds
			if (placeholder == "<Clouds>")
			{
				// Get the next line until we hit the end of the declaration
				while (std::getline(file, line))
				{
					ssLine = std::stringstream(line);
					ssLine >> placeholder;

					if (placeholder == "</Clouds>")
						break;

					if (placeholder == "<MovementSpeedX>")
					{
						// Extract the movement speed from the file
						ssLine >> mCloudMovementSpeed.x;
					}
					else if (placeholder == "<MovementSpeedY>")
					{
						// Extract the movement speed from the file
						ssLine >> mCloudMovementSpeed.y;
					}
					else if (placeholder == "<GlobalCoverage>")
					{
						// Extract the coverage factor from the file
						ssLine >> mGlobalCoverage;
					}

					continue;
				}

				continue;
			}

			if (placeholder == "<Type>")
			{
				continue;
			}

			if (placeholder == "</Weather>")
				break;
		}
	}

	currentFilePos = file.tellg();

	file.close();

	return currentFilePos;
}

// -------------------------------------------------------------------------------- //

std::streampos RaceTrack::LoadInTrackData(std::ifstream& file, std::string filePath, std::streampos currentFilePos)
{
	// First re-open the file
	file.open(filePath.c_str(), std::ios::in);

	std::string       line;
	std::string       placeholder;
	std::stringstream ssLine;
	unsigned int      trackID;

	// Error checking
	if (file.is_open() && file.good())
	{
		// Go back to where the prior function left off
		file.seekg(currentFilePos);

		unsigned int row = 0, col = 0;

		while (std::getline(file, line))
		{
			ssLine =  std::stringstream(line);
			ssLine >> placeholder;

			if (placeholder == "</TrackData>")
				break;

			// Refresh the string stream so that it doesnt lose the first value in the grid
			ssLine = std::stringstream(line);

			col = 0;

			while (ssLine >> trackID)
			{
				mRaceTrack->SetGridPiece((TrackPieceType)trackID, 0, Vector2D((float)row, (float)col));

				col++;
			}

			row++;
		}
	}

	currentFilePos = file.tellg();

	file.close();

	return currentFilePos;
}

// -------------------------------------------------------------------------------- //