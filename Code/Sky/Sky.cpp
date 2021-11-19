#include "Sky.h"

#include "../Maths/Commons.h"

// ---------------------------------------------------------------- 

// X slices is how many steps up the angle must go before reaching straight up
// Y slices is how many steps around the angle must go before reaching back to where it started
SkyDome::SkyDome(ShaderHandler& shaderHandler, Vector3D centre, float radius, unsigned int divisions)
	: mDomeCentre(centre)
	, mRadius(radius)
	, mDivisions(divisions)
	, mVertexShader(nullptr)
	, mPixelShader(nullptr)
	, mVertexBuffer(nullptr)
	, mIndexBuffer(nullptr)
	, mConstantBuffer(nullptr)
	, mShaderHandler(shaderHandler)
	, mInputLayout(nullptr)
	, mModelMat(MatrixMaths::Identity4X4)
	, renderState(nullptr)
{
	// First generate the dome
	GenerateDome();

	// Now setup the shaders requred for rendering the dome
	SetupShaders();

	mShaderHandler.CreateRasterizerState(&renderState, D3D11_FILL_WIREFRAME, D3D11_CULL_FRONT);
}

// ---------------------------------------------------------------- 

SkyDome::~SkyDome()
{
	if (mVertexShader)
	{
		mVertexShader->Release();
		mVertexShader = nullptr;
	}

	if (mPixelShader)
	{
		mPixelShader->Release();
		mPixelShader = nullptr;
	}

	if (mVertexBuffer)
	{
		mVertexBuffer->Release();
		mVertexBuffer = nullptr;
	}

	if (mIndexBuffer)
	{
		mIndexBuffer->Release();
		mIndexBuffer = nullptr;
	}

	if (mInputLayout)
	{
		mInputLayout->Release();
		mInputLayout = nullptr;
	}
}

// ---------------------------------------------------------------- 

void SkyDome::Render(BaseCamera* camera)
{
	// Quick outs
	if (!mVertexShader || !mPixelShader || !mInputLayout ||!camera)
		return;

	// Make the dome render in wireframe mode
	mShaderHandler.BindRasterizerState(renderState);

	// Set the input layout for this render
	mShaderHandler.SetInputLayout(mInputLayout);

	// Set the shaders
	mShaderHandler.SetVertexShader(mVertexShader);
	mShaderHandler.SetPixelShader(mPixelShader);
	
	// Bind the vertex buffer
	unsigned int offset = 0;
	mShaderHandler.BindVertexBuffersToRegisters(0, 1, &mVertexBuffer, &mVertexBufferStride, &offset);

	// Now bind the index buffer
	mShaderHandler.BindIndexBuffersToRegisters(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Now setup the constant buffer data
	ConstantBuffer cb;

	// Get the stored world matrix for the dome
	DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&mModelMat);
	              cb.mWorld = DirectX::XMMatrixTranspose(world);

	// Get the view and projection matricies from the camera
	DirectX::XMMATRIX   view;
	DirectX::XMFLOAT4X4 view4X4  = camera->GetViewMatrix();
					    view     = DirectX::XMLoadFloat4x4(&view4X4);
	                    cb.mView = DirectX::XMMatrixTranspose(view);

	DirectX::XMMATRIX   projection;
	DirectX::XMFLOAT4X4 persp          = camera->GetPerspectiveMatrix();
					    projection     = DirectX::XMLoadFloat4x4(&persp);
					    cb.mProjection = DirectX::XMMatrixTranspose(projection);

				     cb.cameraPosition = camera->GetPosition();

	// Now send the data to the shader
	mShaderHandler.UpdateSubresource(mConstantBuffer, 0, nullptr, &cb, 0, 0);
	mShaderHandler.SetVertexShaderConstantBufferData(0, 1, &mConstantBuffer);
	mShaderHandler.SetPixelShaderConstantBufferData(0, 1, &mConstantBuffer);

	// For this render we are going to use triangle strips as it is simpler to define a dome using triangle strips than triangle lists
	if (!mShaderHandler.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST))
		return;

		// Now draw the dome using the indicies
		mShaderHandler.DrawIndexed(mIndexCount, 0, 0);

	// Now unbind the shaders
	mShaderHandler.SetVertexShader(nullptr);
	mShaderHandler.SetPixelShader(nullptr);
}

// ---------------------------------------------------------------- 

void SkyDome::Update(const float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);
}

// ---------------------------------------------------------------- 

// X slices is how many steps up the angle must go before reaching straight up
// Y slices is how many steps around the angle must go before reaching back to where it started
void SkyDome::GenerateDome()
{
	CalculateVerticies();

	CalculateIndicies();
}

// ---------------------------------------------------------------- 

void SkyDome::SetupShaders()
{
	// Generate the vertex shader
	VertexShaderReturnData returnData = mShaderHandler.CompileVertexShader(L"SkyDomeRender.fx", "VS");
		                mVertexShader = returnData.vertexShader;

	// Generate the pixel shader
    mPixelShader = mShaderHandler.CompilePixelShader(L"SkyDomeRender.fx", "PS");

	// Position, normal and texture coord
	D3D11_INPUT_ELEMENT_DESC fullRenderLayout[] =
	{
		{ "POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Now setup the input layout for the shader
	mShaderHandler.SetDeviceInputLayout(returnData.Blob, fullRenderLayout, 1, &mInputLayout);

	// Create the constant buffer
	if (!mShaderHandler.CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, (D3D11_CPU_ACCESS_FLAG)0, nullptr, sizeof(ConstantBuffer), &mConstantBuffer))
		return;
}

// ---------------------------------------------------------------- 

void SkyDome::CalculateVerticies()
{
	// All verticies are the same distance from the centre of the sphere (the radius away)
	// Using spherical coordinates we can calculate all of the vertices equidist from each other

	float deltaLatitude  = PI    / (float)mDivisions; // How up/down we are pointing
	float deltaLongitude = TWOPI / (float)mDivisions; // How far around the object we are pointing

	// Current angles being pointed at
	float currentLatitude  = 0.0f;
	float currentLongitude = 0.0f;

	// First add the top most vertex, as we dont want to add it a lot of times
	std::vector<Vector3D> vertexData;
	vertexData.push_back(Vector3D(0.0f, mRadius, 0.0f));

	// What we want to do is rotate a full rotation around the same latitude before changing the latitude
	// = 1 and -1 to account for the fact we are pre-post adding the top and bottom of the circle
	for (unsigned int i = 1; i < mDivisions - 1; i++)
	{
		// Reset the longitude as it should currently either be 0 or 2PI
		currentLongitude = 0.0f;

		// Now we have completed a loop we can point more upwards
		currentLatitude += deltaLatitude;

		// Now loop around the sphere's longitude for a full rotation
		for (unsigned int j = 0; j < mDivisions; j++)
		{
			// Now given the current angles calculate where we are pointing using spherical coordinates
			vertexData.push_back(Vector3D( mRadius * cosf(currentLongitude) * sinf(currentLatitude),
				                           mRadius * cosf(currentLatitude), // The Y value should go from +radius to -radius as the latitide changes
				                           mRadius * sinf(currentLatitude)  * sinf(currentLongitude)));

			// Increment the longitude so we can loop around the sphere completly
			currentLongitude += deltaLongitude;
		}
	}

	// Now add the last vertex so we dont add it lots of times
	vertexData.push_back(Vector3D(0.0f, -mRadius, 0.0f));

	mVertexBufferStride = sizeof(Vector3D);
	mVertexCount        = vertexData.size();
	if (!mShaderHandler.CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, (D3D11_CPU_ACCESS_FLAG)0, vertexData.data(), sizeof(Vector3D) * mVertexCount, &mVertexBuffer))
		return;

	vertexData.clear();
}

// ---------------------------------------------------------------- 

void SkyDome::CalculateIndicies()
{
	// Calulation of indicies in order to render a sphere

	std::vector<unsigned int> indexData;

	// Now calculate the indices for the triangles
	// Start with the top ring of triangles around the pole, as the top is stored as a single point instead of a load of points in the same place
	for (unsigned int i = 0; i < mDivisions; i++)
	{
		// For each of these triangles - 0 will be the first value as they all link to the top
		indexData.push_back(0);

		// Now add the current index on the first loop
		indexData.push_back(i + 1);

		// Now add the index next to the previous index on the same loop
		indexData.push_back(((i + 1) % mDivisions) + 1);
	}

	unsigned int currentIndexID;

	// Now calculate the filling triangles
	for (unsigned int i = 0; i < mDivisions - 3; i++)
	{ 
		// Calculate the starting offset we are at (+1 to account for the top, i * divisions so that we move down the correct amount of loops)
		currentIndexID = (i * mDivisions) + 1;

		for (unsigned int j = 0; j < mDivisions; j++)
		{
			// Start off with the current ID of the loop we are on
			indexData.push_back(currentIndexID + j);

			// Now add the id of the next index of the same loop we are on
			indexData.push_back(currentIndexID + ((j + 1) % mDivisions));

			// Now add the ID of the index on the loop below ours
			indexData.push_back((currentIndexID + mDivisions) + j);
		}
	}
	
	unsigned int endIndexID = mVertexCount - 1;

	// Now calculate the bottom most ring of triangles
	for (unsigned int i = 0; i < mDivisions; i++)
	{
		// For each of these triangles the last index will be the first value as they all link to the top
		indexData.push_back(endIndexID);

		// Now add the current index on the loop above the end point
		indexData.push_back(endIndexID - ((i % mDivisions) + 1));

		// Now add the index next to the previous index on the same loop
		indexData.push_back(endIndexID - (((i + 1) % mDivisions) + 1));
	}

	mIndexCount = indexData.size();
	if (!mShaderHandler.CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, (D3D11_CPU_ACCESS_FLAG)0, indexData.data(), sizeof(unsigned int) * mIndexCount, &mIndexBuffer))
		return;

	indexData.clear();
}

// ---------------------------------------------------------------- 