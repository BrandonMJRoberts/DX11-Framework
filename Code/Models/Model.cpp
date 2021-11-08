#include "Model.h"

#include <fstream>

#include "../Shaders/ShaderHandler.h"

// --------------------------------------------------------- //

Model::Model(ShaderHandler&      shaderHandler, 
	         std::string         filePathToLoadFrom, 
	         ID3D11VertexShader* geometryRenderVertexShader, 
	         ID3D11PixelShader*  geometryRenderPixelShader, 
	         ID3DBlob*           geometryBlob,

	         ID3D11VertexShader* fullRenderVertexShader, 
	         ID3D11PixelShader*  fullRenderPixelShader,
	         ID3DBlob*           fullRenderBlob)
	:   mShaderHandler(shaderHandler)
	  , mFaceData(nullptr)
	  , mFullRenderVertexShader(geometryRenderVertexShader)
	  , mFullRenderPixelShader(geometryRenderPixelShader)
	  , mGeometryRenderVertexShader(fullRenderVertexShader)
	  , mGeometryRenderPixelShader(fullRenderPixelShader)
	  , mFullRenderInputLayout(nullptr)
	  , mGeometryInputLayout(nullptr)
	  , mVertexBuffer(nullptr)
	  , mVertexCount(0)
	  , mTexture(nullptr)
	  , mSamplerState(nullptr)
{
	// First clear all data stored
	RemoveAllPriorDataStored();

	// Now load in the data from the file
	if (filePathToLoadFrom != "")
		LoadInModelFromFile(filePathToLoadFrom);

	// Now create the input layouts
	SetupInputLayouts(geometryBlob, fullRenderBlob);

	mSamplerState = new SamplerState(mShaderHandler, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, 0.0f, 0, 1, 0.0f, 0.0f, 0.0f, 0.0f, D3D11_COMPARISON_ALWAYS);
}

// --------------------------------------------------------- //

Model::~Model()
{
	if (mFullRenderVertexShader)
	{
		mFullRenderVertexShader->Release();
		mFullRenderVertexShader = nullptr;
	}

	if (mFullRenderPixelShader)
	{
		mFullRenderPixelShader->Release();
		mFullRenderPixelShader = nullptr;
	}

	if (mGeometryRenderVertexShader)
	{
		mGeometryRenderVertexShader->Release();
		mGeometryRenderVertexShader = nullptr;
	}

	if (mGeometryRenderPixelShader)
	{
		mGeometryRenderPixelShader->Release();
		mGeometryRenderPixelShader = nullptr;
	}

	if (mVertexBuffer)
	{
		mVertexBuffer->Release();
		mVertexBuffer = nullptr;
	}

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
	unsigned int normalCount = 0, textureCoordCount = 0, faceCount = 0;
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
			mVertexCount++;
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
	vertexPositions       = new Vector3D[mVertexCount];
	vertexNormals         = new Vector3D[normalCount];
	textureCoordPositions = new Vector2D[textureCoordCount];
	mFaceData             = new FaceData[faceCount];

	mVertexCount = 0;
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
			vertexPositions[mVertexCount++] = ExtractThreeDataPointsFromLine(subData);
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

	// Now for the vertex buffer
	if (!mShaderHandler.CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, (D3D11_CPU_ACCESS_FLAG)0, mFaceData, sizeof(FaceData) * faceCount, &mVertexBuffer))
		return false;

	UINT stride = sizeof(FaceData);
	UINT offset = 0;

	if (!mShaderHandler.BindVertexBuffersToRegisters(0, 1, &mVertexBuffer, &stride, &offset))
		return false;

	// ----------------------------------------------------------------------------------------------------------- //

	// Now load in the material data and the textures required
	//LoadInMaterialData();

	// ----------------------------------------------------------------------------------------------------------- //

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
	if (!mGeometryRenderVertexShader || !mGeometryRenderPixelShader)
		return;
}

// --------------------------------------------------------- //

void Model::FullRender()
{
	if (!mFullRenderVertexShader || !mFullRenderPixelShader)
		return;

	// First set the input layout
	mShaderHandler.SetInputLayout(mFullRenderInputLayout);

	// Bind the shaders
	mShaderHandler.SetVertexShader(mFullRenderVertexShader);
	mShaderHandler.SetPixelShader(mFullRenderPixelShader);

	// Bind the texture
	if (mTexture)
		mTexture->BindTextureToShaders(0, 1);

	// Set the sampler state
	if (mSamplerState)
		mSamplerState->BindSamplerState(0, 1);

	// Make sure to bind the data we are going to be using
	UINT stride = sizeof(FaceData);
	UINT offset = 0;

	// Bind the vertex buffer to the correct registers
	mShaderHandler.BindVertexBuffersToRegisters(0, 1, &mVertexBuffer, &stride, &offset);

	// Render
	mShaderHandler.Draw(mVertexCount, 0);

	// Unbind
	mShaderHandler.SetVertexShader(nullptr);
	mShaderHandler.SetPixelShader(nullptr);
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
					data[dataIndex++] = (float)std::stoi(subString);

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

void Model::SetShadersForFullRender(ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader)
{
	if (vertexShader)
	{
		if (mFullRenderVertexShader)
		{
			mFullRenderVertexShader->Release();
		}

		mFullRenderVertexShader = vertexShader;
	}

	if (pixelShader)
	{
		if (mFullRenderPixelShader)
		{
			mFullRenderPixelShader->Release();
		}

		mFullRenderPixelShader = pixelShader;
	}
}

// --------------------------------------------------------- //

void Model::SetShadersForGeometryRender(ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader)
{
	if (vertexShader)
	{
		if (mGeometryRenderVertexShader)
		{
			mGeometryRenderVertexShader->Release();
		}

		mGeometryRenderVertexShader = vertexShader;
	}

	if (pixelShader)
	{
		if (mGeometryRenderPixelShader)
		{
			mGeometryRenderPixelShader->Release();
		}

		mGeometryRenderPixelShader = pixelShader;
	}
}

// --------------------------------------------------------- //

void Model::SetupInputLayouts(ID3DBlob* geometryBlob, ID3DBlob* fullRenderBlob)
{
	// Just the position
	D3D11_INPUT_ELEMENT_DESC geometryLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (!mShaderHandler.SetDeviceInputLayout(geometryBlob, geometryLayout, 1, &mGeometryInputLayout))
		return;

	// Position, normal and texture coord
	D3D11_INPUT_ELEMENT_DESC fullRenderLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if (!mShaderHandler.SetDeviceInputLayout(fullRenderBlob, fullRenderLayout, 3, &mFullRenderInputLayout))
		return;
}

// --------------------------------------------------------- //

void Model::LoadInMaterialData(std::string filePath)
{
	 
}

// --------------------------------------------------------- //