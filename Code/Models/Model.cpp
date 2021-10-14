#include "Model.h"

#include <fstream>

#include "../Shaders/ShaderHandler.h"

// --------------------------------------------------------- //

Model::Model(ShaderHandler& shaderHandler, std::string filePathToLoadFrom)
	: mShaderHandler(shaderHandler)
	, mVertexData(nullptr)
	, mIndexData(nullptr)
{
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


	// Close the file
	file.close();

	return true;
}

// --------------------------------------------------------- //

void Model::RemoveAllPriorDataStored()
{
	if (mVertexData)
	{
		delete mVertexData;
		mVertexData = nullptr;
	}

	if (mIndexData)
	{
		delete mIndexData;
		mIndexData = nullptr;
	}
}

// --------------------------------------------------------- //