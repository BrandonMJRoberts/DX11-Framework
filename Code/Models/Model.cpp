#include "Model.h"

#include <fstream>

#include "../Shaders/ShaderHandler.h"

// --------------------------------------------------------- //

Model::Model(ShaderHandler& shaderHandler, std::string filePathToLoadFrom)
	: mShaderHandler(shaderHandler)
	  , mFaceData(nullptr)
{
	// First clear all data stored
	RemoveAllPriorDataStored();

	// Now load in the data from the file
	if (filePathToLoadFrom != "")
		LoadInModelFromFile(filePathToLoadFrom);
}

// --------------------------------------------------------- //

Model::~Model()
{
	RemoveAllPriorDataStored();
}

// --------------------------------------------------------- //

bool Model::LoadInModelFromFile(std::string filePath)
{
	std::fstream file;
	file.open(filePath.c_str());

	// Error checking
	if (!file.is_open() || !file.good())
		return false;

	// Now the file is open now we can load in the data for the model

	// First find how many verticies, normals and textur coords are stored in the file so we can allocate the correct amount of tempoary memory
	unsigned int vertexCount = 0, normalCount = 0, textureCoordCount = 0, faceCount = 0;
	Vector3D* vertexPositions;
	Vector3D* vertexNormals;
	Vector2D* textureCoordPositions;

	std::string line;

	// Loop through the entire file to get the correct counts
	while (std::getline(file, line))
	{
		if (line == "")
			continue;

		if (line[0] == 'f')
		{
			faceCount++;
			continue;
		}

		if (line[0] == 'v' && line[1] == ' ')
		{
			vertexCount++;
			continue;
		}

		if (line[0] == 'v' && line[1] == 't')
		{
			textureCoordCount++;
			continue;
		}

		if (line[0] == 'v' && line[1] == 'n')
		{
			normalCount++;
			continue;
		}
	}

	// Now we know the counts we can allocate the correct amount of data
	vertexPositions       = new Vector3D[vertexCount];
	vertexNormals         = new Vector3D[normalCount];
	textureCoordPositions = new Vector2D[textureCoordCount];
	mFaceData             = new FaceData[faceCount];

	vertexCount       = 0;
	normalCount       = 0;
	textureCoordCount = 0;
	faceCount         = 0;

	std::string subData;

	// As we want to re-go through the file we need to seek to the beginnning of the file again
	file.clear();
	file.seekg(0, std::ios::beg);

	// Now run through the file again and read in the actual data from the line
	while (std::getline(file, line))
	{
		if (line == "")
			continue;

		if (line[0] == 'f')
		{
			mFaceData[faceCount++] = ConstructFaceFromData(line, vertexPositions, vertexNormals, textureCoordPositions);
			continue;
		}

		if (line[0] == 'v' && line[1] == ' ')
		{
			subData = line.substr(2);
			vertexPositions[vertexCount++] = ExtractThreeDataPointsFromLine(subData);
			continue;
		}

		if (line[0] == 'v' && line[1] == 't')
		{
			subData = line.substr(3);
			textureCoordPositions[textureCoordCount++] = ExtractTwoDataPointsFromLine(subData);
			continue;
		}

		if (line[0] == 'v' && line[1] == 'n')
		{
			subData = line.substr(3);
			vertexNormals[normalCount++] = ExtractThreeDataPointsFromLine(subData);
			continue;
		}
	}

	delete[] vertexPositions;
	delete[] vertexNormals;
	delete[] textureCoordPositions;

	// Close the file
	file.close();

	return true;
}

// --------------------------------------------------------- //

void Model::RemoveAllPriorDataStored()
{
	if (mFaceData)
	{
		delete mFaceData;
		mFaceData = nullptr;
	}
}

// --------------------------------------------------------- //

void Model::RenderGeometry()
{

}

// --------------------------------------------------------- //

void Model::FullRender()
{

}

// --------------------------------------------------------- //

Vector3D Model::ExtractThreeDataPointsFromLine(std::string& line)
{
	Vector3D     returnData = Vector3D::zero;
	std::string  subData;
	unsigned int currentDataIndex = 0;

	// Loop through the line until we hit something
	for (unsigned int i = 0; i < line.length(); i++)
	{
		if (line[i] != ' ')
		{
			// Now we have hit something we need to read until we dont hit anything anymore
			for (unsigned int j = i; j < line.length(); j++)
			{
				if (line[j] == ' ' || j == (line.length() - 1))
				{
					// We now have our start and end points for this data point, so extract it from the line
					subData = line.substr(i, j - i);
					returnData[currentDataIndex++] = std::stof(subData);

					// Shift the line along 
					i = j;
					break;
				}
			}
		}
	}

	// Return the data back
	return returnData;
}

// --------------------------------------------------------- //

Vector2D Model::ExtractTwoDataPointsFromLine(std::string& line)
{
	Vector2D     returnData = Vector2D::zero;
	std::string  subData;
	unsigned int currentDataIndex = 0;

	// Loop through the line until we hit something
	for (unsigned int i = 0; i < line.length(); i++)
	{
		if (line[i] != ' ')
		{
			// Now we have hit something we need to read until we dont hit anything anymore
			for (unsigned int j = i; j < line.length(); j++)
			{
				if (line[j] == ' ' || j == (line.length() - 1))
				{
					// We now have our start and end points for this data point, so extract it from the line
					subData = line.substr(i, j - i);
					returnData[currentDataIndex++] = std::stof(subData);

					// Shift the line along 
					i = j;
					break;
				}
			}
		}
	}

	// Return the data back
	return returnData;
}

// --------------------------------------------------------- //

FaceData Model::ConstructFaceFromData(std::string& line, Vector3D* vertexData, Vector3D* normalData, Vector2D* textureCoordData)
{
	FaceData returnData;
	Vector3D indexDataFromFile[3];
	
	// Extract the indicies from the file
	std::string subString = line.substr(2);
	ExtractFaceIndexDataFromFile(subString, indexDataFromFile);

	for (unsigned int i = 0; i < 3; i++)
	{
		// Set the correct data from the store

		// Vertex position
		DirectX::XMStoreFloat3(&returnData.verticies[i].vertexPosition,
			                    DirectX::XMVectorSet(vertexData[(unsigned int)indexDataFromFile[i].x].x, 
									                 vertexData[(unsigned int)indexDataFromFile[i].x].y, 
									                 vertexData[(unsigned int)indexDataFromFile[i].x].z, 
													 0.0f));

		// Vertex normal
		DirectX::XMStoreFloat3(&returnData.verticies[i].normal, 
			                    DirectX::XMVectorSet(normalData[(unsigned int)indexDataFromFile[i].x].x, 
									                 normalData[(unsigned int)indexDataFromFile[i].x].y, 
									                 normalData[(unsigned int)indexDataFromFile[i].x].z,  
									                 0.0f));

		// Vertex texture coord
		DirectX::XMStoreFloat2(&returnData.verticies[i].textureCoord,
			                    DirectX::XMVectorSet(textureCoordData[(unsigned int)indexDataFromFile[i].x].x,
									                 textureCoordData[(unsigned int)indexDataFromFile[i].x].y,
									                 0.0f, 
									                 0.0f));
	}

	// Return it back
	return returnData;
}

// --------------------------------------------------------- //

void     Model::ExtractFaceIndexDataFromFile(std::string& line, Vector3D indexDataFromFile[3])
{
	Vector3D     data             = Vector3D::zero;
	unsigned int dataIndex        = 0;
	unsigned int overallDataIndex = 0;
	std::string  subString;

	// Loop through the line and extract the indices between the '/'s
	for (unsigned int i = 0; i < line.length(); i++)
	{
		if (line[i] != ' ')
		{
			// Reset the index
			dataIndex = 0;

			for (unsigned int j = i; j <= line.length(); j++)
			{
				if (line[j] == '/' || line[j] == ' ' || j == line.length())
				{
					subString         = line.substr(i, j - i);
					data[dataIndex++] = std::stoi(subString);

					i = j + 1;

					if (line[j] == ' ' || j == line.length())
					{
						// Pass the data back out
						indexDataFromFile[overallDataIndex++] = data;

						// Zero the internal data store
						data = Vector3D::zero;

						// Move the index along
						i = j;
						break;
					}

					continue;
				}
			}
		}
	}
}

// --------------------------------------------------------- //