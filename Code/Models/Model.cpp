#include "Model.h"

#include <fstream>

#include "../Shaders/ShaderHandler.h"
#include "../Maths/Commons.h"

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
	  , mFullRenderVertexShader(fullRenderVertexShader)
	  , mFullRenderPixelShader(fullRenderPixelShader)
	  , mGeometryRenderVertexShader(geometryRenderVertexShader)
	  , mGeometryRenderPixelShader(geometryRenderPixelShader)
	  , mFullRenderInputLayout(nullptr)
	  , mGeometryInputLayout(nullptr)
	  , mVertexBuffer(nullptr)
	  , mVertexCount(0)
	  , mSamplerState(nullptr)
	  , mConstantBuffer(nullptr)
	  , mVertexBufferStride(0)
	  , mVertexBufferOffset(0)
{
	// First clear all data stored
	RemoveAllPriorDataStored();

	// Now load in the data from the file
	if (filePathToLoadFrom != "")
		LoadInModelFromFile(filePathToLoadFrom);

	// Now create the input layouts
	SetupInputLayouts(geometryBlob, fullRenderBlob);

	mSamplerState = new SamplerState(mShaderHandler, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, 0.0f, 0, 1, 0.0f, 0.0f, 0.0f, 0.0f, D3D11_COMPARISON_ALWAYS);

	// Now for the constant buffer
	if (!shaderHandler.CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, (D3D11_CPU_ACCESS_FLAG)0, nullptr, sizeof(ConstantBuffer), &mConstantBuffer))
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

	if (mConstantBuffer)
	{
		mConstantBuffer->Release();
		mConstantBuffer = nullptr;
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
	unsigned int normalCount = 0, textureCoordCount = 0, faceCount = 0, vertexCount = 0;
	Vector3D* vertexPositions;
	Vector3D* vertexNormals;
	Vector2D* textureCoordPositions;

	std::string line;
	std::string materialLibraryName;

	// Loop through the entire file to get the correct counts
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

	// There are three verticies per face - the vertex count above is unique verticies, not renderable verticies
	mVertexCount = faceCount * 3;

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
		delete mFaceData;
		mFaceData = nullptr;
	}
}

// --------------------------------------------------------- //

void Model::RenderGeometry(BaseCamera* camera, const DirectX::XMFLOAT4X4 modelMat)
{
	if (!mGeometryRenderVertexShader || !mGeometryRenderPixelShader)
		return;
}

// --------------------------------------------------------- //

void Model::FullRender(BaseCamera* camera, const DirectX::XMFLOAT4X4 modelMat)
{
	if (!mFullRenderVertexShader || !mFullRenderPixelShader)
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
	if (mMaterialData.size() > 0 && mMaterialData[0] && mMaterialData[0]->texture)
		mMaterialData[0]->texture->BindTextureToShaders(0, 1);

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

	// Update the constant buffer
	mShaderHandler.UpdateSubresource(mConstantBuffer, 0, nullptr, &cb, 0, 0);

	// Set the data in the shaders
	mShaderHandler.SetVertexShaderConstantBufferData(0, 1, &mConstantBuffer);
	mShaderHandler.SetPixelShaderConstantBufferData(0, 1, &mConstantBuffer);

	// ------------------------------------------------------------------------------ 

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
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if (!mShaderHandler.SetDeviceInputLayout(fullRenderBlob, fullRenderLayout, 3, &mFullRenderInputLayout))
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
					newData->ambient = ExtractThreeDataPointsFromLine(subStr);
					continue;
				}
				else if (line[1] == 'd')
				{
					subStr = line.substr(3);
					newData->diffuse = ExtractThreeDataPointsFromLine(subStr);
					continue;
				}
				else if (line[1] == 's')
				{
					subStr = line.substr(3);
					newData->specular = ExtractThreeDataPointsFromLine(subStr);
					continue;
				}
			}

			// Extract the texture file from the file
			if (line[0] == 'm')
			{
				// Create the initial data for the texture
				newData->texture = new Texture2D(mShaderHandler);

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
				newData->texture->LoadTextureInFromFile(buffer);

				delete[] buffer;

				continue;
			}

			// Get the specular intensity multiplier value
			if (line[0] == 'i')
			{
				
			}
		}

		// Make sure to push back the last material defined
		mMaterialData.push_back(newData);
	}

	materialFile.close();
}

// --------------------------------------------------------- //