#include "Model.h"

#include <fstream>

#include "../Shaders/ShaderHandler.h"
#include "../Maths/Commons.h"

// --------------------------------------------------------- //

Model::Model(ShaderHandler&      shaderHandler, 
	         std::string         filePathToLoadFrom, 

	         ID3D11VertexShader* fullRenderVertexShader, 
	         ID3D11PixelShader*  fullRenderPixelShader,
	         ID3DBlob*           fullRenderBlob,
	         bool                clearBlob)
	:   mShaderHandler(shaderHandler)
	  , mFaceData(nullptr)

	  , mFullRenderVertexShader(fullRenderVertexShader)
	  , mFullRenderPixelShader(fullRenderPixelShader)

	  , mFullRenderInputLayout(nullptr)

	  , mVertexBuffer(nullptr)
	  , mVertexCount(0)

	  , mSamplerState(nullptr)

	  , mMatrixConstantBuffer(nullptr)
	  , mLightConstantBuffer(nullptr)
	  , mMaterialConstantBuffer(nullptr)

	  , mVertexBufferStride(0)
	  , mVertexBufferOffset(0)
	  , placeHolderTexture(nullptr)
{
	// First clear all data stored
	RemoveAllPriorDataStored();

	// Now load in the data from the file
	if (filePathToLoadFrom != "")
		LoadInModelFromFile(filePathToLoadFrom);

	// Now create the input layouts
	SetupInputLayouts(fullRenderBlob, clearBlob);

	mSamplerState = new SamplerState(mShaderHandler, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, 0.0f, 0, 1, 0.0f, 0.0f, 0.0f, 0.0f, D3D11_COMPARISON_ALWAYS);

	// Now for the constant buffers
	if (!shaderHandler.CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, (D3D11_CPU_ACCESS_FLAG)0, nullptr, sizeof(ConstantBuffer), &mMatrixConstantBuffer))
		return;

	if (!shaderHandler.CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, (D3D11_CPU_ACCESS_FLAG)0, nullptr, sizeof(BasicMaterialData), &mMaterialConstantBuffer))
		return;

	if (!shaderHandler.CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, (D3D11_CPU_ACCESS_FLAG)0, nullptr, sizeof(BasicDirectionalLightingData), &mLightConstantBuffer))
		return;
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

	if (mVertexBuffer)
	{
		mVertexBuffer->Release();
		mVertexBuffer = nullptr;
	}

	if (mMatrixConstantBuffer)
	{
		mMatrixConstantBuffer->Release();
		mMatrixConstantBuffer = nullptr;
	}

	if (mLightConstantBuffer)
	{
		mLightConstantBuffer->Release();
		mLightConstantBuffer = nullptr;
	}

	if (mMaterialConstantBuffer)
	{
		mMaterialConstantBuffer->Release();
		mMaterialConstantBuffer = nullptr;
	}

	RemoveAllPriorDataStored();

	for (unsigned int i = 0; i < mMaterialData.size(); i++)
	{
		delete mMaterialData[i];
		mMaterialData[i] = nullptr;
	}

	mMaterialData.clear();
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
	unsigned int normalCount = 0, textureCoordCount = 0, faceCount = 0, uniqueVertexCount = 0;
	Vector3D* vertexPositions;
	Vector3D* vertexNormals;
	Vector2D* textureCoordPositions;

	std::string line;
	std::string materialLibraryName;

	// Loop through the entire file to get the correct counts of each element
	while (std::getline(file, line))
	{
		if (line == "")
			continue;

		// The material libray definition
		if (line[0] == 'm' && line[1] == 't')
		{
			// Get the name of the material library
			for (unsigned int i = 0; i < line.length(); i++)
			{
				if (line[i] == ' ')
				{
					materialLibraryName = line.substr(i + 1);
					break;
				}
			}
		}

		// Face count
		if (line[0] == 'f')
		{
			faceCount++;
			continue;
		}

		// Vertex position count
		if (line[0] == 'v' && line[1] == ' ')
		{
			uniqueVertexCount++;
			continue;
		}

		// Texture coord count
		if (line[0] == 'v' && line[1] == 't')
		{
			textureCoordCount++;
			continue;
		}

		// Normals count
		if (line[0] == 'v' && line[1] == 'n')
		{
			normalCount++;
			continue;
		}
	}

	// Now we know the counts we can allocate the correct amount of data
	vertexPositions       = new Vector3D[uniqueVertexCount];
	vertexNormals         = new Vector3D[normalCount];
	textureCoordPositions = new Vector2D[textureCoordCount];

	// Allocate the correct amount of memory for the vertex data
	mVertexCount = faceCount * 3;
	mFaceData    = new FaceData[faceCount];

	std::string subData;
	unsigned int currentVertexBeingExtracted = 0, currentNormal = 0, currentTextureCoord = 0, currentFaceBeingExtracted = 0;


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
			// Extract the vertex data
			ConstructFaceFromData(line, vertexPositions, vertexNormals, textureCoordPositions, mFaceData[currentFaceBeingExtracted++]);
			continue;
		}

		if (line[0] == 'v' && line[1] == ' ')
		{
			subData = line.substr(2);
			vertexPositions[currentVertexBeingExtracted++] = ExtractThreeDataPointsFromLine(subData);
			continue;
		}

		if (line[0] == 'v' && line[1] == 't')
		{
			subData = line.substr(3);
			textureCoordPositions[currentNormal++] = ExtractTwoDataPointsFromLine(subData);
			continue;
		}

		if (line[0] == 'v' && line[1] == 'n')
		{
			subData = line.substr(3);
			vertexNormals[currentTextureCoord++] = ExtractThreeDataPointsFromLine(subData);
			continue;
		}
	}

	// Clear up the tempoary data
	delete[] vertexPositions;       vertexPositions       = nullptr;
	delete[] vertexNormals;         vertexNormals         = nullptr;
	delete[] textureCoordPositions; textureCoordPositions = nullptr;

	// Close the file
	file.close();

	// Now for the vertex buffer
	if (!mShaderHandler.CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, (D3D11_CPU_ACCESS_FLAG)0, mFaceData, sizeof(VertexData) * mVertexCount, &mVertexBuffer))
		return false;

	mVertexBufferStride = sizeof(VertexData);
	mVertexBufferOffset = 0;

	mShaderHandler.SetVertexShader(mFullRenderVertexShader);

	if (!mShaderHandler.BindVertexBuffersToRegisters(0, 1, &mVertexBuffer, &mVertexBufferStride, &mVertexBufferOffset))
		return false;

	// ----------------------------------------------------------------------------------------------------------- //

	// Now load in the material data and the textures required
	LoadInMaterialData(materialLibraryName);

	// ----------------------------------------------------------------------------------------------------------- //

	return true;
}

// --------------------------------------------------------- //

void Model::RemoveAllPriorDataStored()
{
	if (mFaceData)
	{
		delete[] mFaceData;
		mFaceData = nullptr;
	}
}

// --------------------------------------------------------- //

void Model::FullRender(BaseCamera* camera, const DirectX::XMFLOAT4X4 modelMat)
{
	if (!mFullRenderVertexShader || !mFullRenderPixelShader || !mFullRenderInputLayout)
		return;

	// ------------------------------------------------------------------------------ 

	// First set the input layout
	mShaderHandler.SetInputLayout(mFullRenderInputLayout);

	// ------------------------------------------------------------------------------ 

	// Bind the shaders
	mShaderHandler.SetVertexShader(mFullRenderVertexShader);
	mShaderHandler.SetPixelShader(mFullRenderPixelShader);

	// ------------------------------------------------------------------------------ 

	// Bind the texture
	//if (mMaterialData.size() > 0 && mMaterialData[0] && mMaterialData[0]->texture)
		//mMaterialData[0]->texture->BindTextureToShaders(0, 1);

	if (placeHolderTexture)
	{
		placeHolderTexture->BindTextureToShaders(0, 1);
	}

	// Set the sampler state
	if (mSamplerState)
		mSamplerState->BindSamplerState(0, 1);

	// Bind the vertex buffer to the correct registers
	mShaderHandler.BindVertexBuffersToRegisters(0, 1, &mVertexBuffer, &mVertexBufferStride, &mVertexBufferOffset);

	// ------------------------------------------------------------------------------ 

	// Get the local stored world matrix for this object
	DirectX::XMMATRIX world      = DirectX::XMLoadFloat4x4(&modelMat);

	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(world);

	// Get the view and projection matrixes from the camera and transpose them before passing into the shader
	DirectX::XMMATRIX   view;
	DirectX::XMFLOAT4X4 view4X4  = camera->GetViewMatrix();
					    view     = DirectX::XMLoadFloat4x4(&view4X4);
	                    cb.mView = DirectX::XMMatrixTranspose(view);

	DirectX::XMMATRIX   projection;
	DirectX::XMFLOAT4X4 persp          = camera->GetPerspectiveMatrix();
					    projection     = DirectX::XMLoadFloat4x4(&persp);
					    cb.mProjection = DirectX::XMMatrixTranspose(projection);

						cb.cameraPosition = camera->GetPosition();

	BasicDirectionalLightingData lightingData;
	lightingData.colour    = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	lightingData.direction = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);

	// Update the constant buffer
	mShaderHandler.UpdateSubresource(mMatrixConstantBuffer,   0, nullptr, &cb, 0, 0);
	mShaderHandler.UpdateSubresource(mLightConstantBuffer,    0, nullptr, &lightingData, 0, 0);

	if(mMaterialData.size() > 0)
		mShaderHandler.UpdateSubresource(mMaterialConstantBuffer, 0, nullptr, &(*mMaterialData[0]), 0, 0);

	// Set the data in the shaders
	mShaderHandler.SetVertexShaderConstantBufferData(0, 1, &mMatrixConstantBuffer);
	mShaderHandler.SetPixelShaderConstantBufferData(0, 1, &mMatrixConstantBuffer);

	mShaderHandler.SetVertexShaderConstantBufferData(1, 1, &mLightConstantBuffer);
	mShaderHandler.SetPixelShaderConstantBufferData(1, 1, &mLightConstantBuffer);

	mShaderHandler.SetVertexShaderConstantBufferData(2, 1, &mMaterialConstantBuffer);
	mShaderHandler.SetPixelShaderConstantBufferData(2, 1, &mMaterialConstantBuffer);

	// ------------------------------------------------------------------------------ 

	if (!mShaderHandler.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST))
		return;

	// Render
	mShaderHandler.Draw(mVertexCount, 0);

	// ------------------------------------------------------------------------------ 

	// Unbind the texture
	mShaderHandler.UnbindTextureFromShaderIndex(0, 1);

	// Unbind
	mShaderHandler.SetVertexShader(nullptr);
	mShaderHandler.SetPixelShader(nullptr);

	// ------------------------------------------------------------------------------ 
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

void Model::ConstructFaceFromData(std::string& line, Vector3D* vertexData, Vector3D* normalData, Vector2D* textureCoordData, FaceData& faceBeingReturned)
{
	// Extract the index data from the file
	Vector3D    indexDataFromFile[3];
	std::string subString = line.substr(2);

	// The index order of a .obj file is vertexPos/texture Coord/Normal
	ExtractFaceIndexDataFromFile(subString, indexDataFromFile);

	Vector3D currentData;
	Vector2D currentTextureCoordData;

	// Now loop through the three verticies in the face and set the correct data
	for (unsigned int i = 0; i < 3; i++)
	{
		// Vertex position
		currentData                                   = vertexData[(unsigned int)(indexDataFromFile[i].x - 1)];
		faceBeingReturned.verticies[i].vertexPosition = DirectX::XMFLOAT3(currentData.x, currentData.y, currentData.z);

		// Vertex normal
		currentData                           = normalData[(unsigned int)(indexDataFromFile[i].z - 1)];
		faceBeingReturned.verticies[i].normal = DirectX::XMFLOAT3(currentData.x, currentData.y, currentData.z);

		// Vertex texture coord
		currentTextureCoordData                     = textureCoordData[(unsigned int)(indexDataFromFile[i].y - 1)];
		faceBeingReturned.verticies[i].textureCoord = DirectX::XMFLOAT2(currentTextureCoordData.x, 1.0f - currentTextureCoordData.y);
	}
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

void Model::SetupInputLayouts(ID3DBlob* fullRenderBlob, bool clearBlob)
{
	// Position, normal and texture coord
	D3D11_INPUT_ELEMENT_DESC fullRenderLayout[] =
	{
		{ "POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if (!mShaderHandler.SetDeviceInputLayout(fullRenderBlob, fullRenderLayout, 3, &mFullRenderInputLayout, clearBlob))
		return;

	if (!mShaderHandler.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST))
		return;
}

// --------------------------------------------------------- //

void Model::LoadInMaterialData(std::string filePath)
{
	 // First open the file
	std::ifstream materialFile;
	materialFile.open(filePath);

	bool startedDefinition = false;

	// Check if we can read from it
	if (materialFile.is_open() && materialFile.good())
	{
		std::string line;
		std::string subStr;

		MaterialData* newData = new MaterialData();

		// Loop through the entire file to get the correct counts
		while (std::getline(materialFile, line))
		{
			// Quick outs
			if (line == "")
				continue;

			if (line[0] == '#')
				continue;

			// Check if a new material is being defined
			if (line[0] == 'n')
			{
				// If we have already defined a material then add this one to the list and create a new one
				if (startedDefinition)
				{
					// Add the new material to the list
					mMaterialData.push_back(newData);
				}

				newData           = nullptr;
				newData           = new MaterialData();

				startedDefinition = true;
			}

			// Must be either ambient, specular or diffuse factors
			if (line[0] == 'K')
			{
				if (line[1] == 'a')
				{
					subStr = line.substr(3);
					newData->ambient = Vector4D(ExtractThreeDataPointsFromLine(subStr), 1.0f);
					continue;
				}
				else if (line[1] == 'd')
				{
					subStr = line.substr(3);
					newData->diffuse = Vector4D(ExtractThreeDataPointsFromLine(subStr), 1.0f);
					continue;
				}
				else if (line[1] == 's')
				{
					subStr = line.substr(3);
					newData->specular = Vector4D(ExtractThreeDataPointsFromLine(subStr), 1.0f);
					continue;
				}
			}

			// Extract the texture file from the file
			if (line[0] == 'm')
			{
				// Create the initial data for the texture
				//newData->texture = new Texture2D(mShaderHandler);
				placeHolderTexture = new Texture2D(mShaderHandler);

				// Get the file path from the file by finding the gap between the 'map_Kd' and the file path
				for (unsigned int i = 0; i < line.length(); i++)
				{
					if (line[i] == ' ')
					{
						subStr = line.substr(i + 1);
						break;
					}
				}

				
				// Convert the string to the format windows wants
				const char* charPtr = subStr.c_str();
				size_t size = mbsrtowcs(NULL, &charPtr, 0, NULL);

				wchar_t* buffer = new wchar_t[size + 1]();

				mbsrtowcs(buffer, &charPtr, size + 1, NULL);

				// Now load the texture in from the file
				//newData->texture->LoadTextureInFromFile(buffer);
				placeHolderTexture->LoadTextureInFromFile(buffer);

				delete[] buffer;

				continue;
			}

			// Get the specular intensity multiplier value
			if (line[0] == 'i')
			{
				// Find the first space in the line and extract the data from there
				for (unsigned int i = 0; i < line.length(); i++)
				{
					if (line[i] == ' ')
					{
						subStr = line.substr(i + 1, line.length() - i);
						break;
					}
				}

				// Now convert from the string to an int
				newData->specularPower = std::stof(subStr);
				continue;
			}
		}

		// Make sure to push back the last material defined
		mMaterialData.push_back(newData);
	}

	materialFile.close();
}

// --------------------------------------------------------- //