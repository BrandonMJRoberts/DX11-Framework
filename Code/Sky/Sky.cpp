#include "Sky.h"

#include "../Maths/Commons.h"

// ---------------------------------------------------------------- 

// X slices is how many steps up the angle must go before reaching straight up
// Y slices is how many steps around the angle must go before reaching back to where it started
SkyDome::SkyDome(ShaderHandler& shaderHandler, Vector3D centre, float radius, unsigned int slicesX, unsigned int slicesY)
	: mDomeCentre(centre)
	, mRadius(radius)
	, mSlicesX(slicesX)
	, mSlicesY(slicesY)
	, mVertexShader(nullptr)
	, mPixelShader(nullptr)
	, mVertexBuffer(nullptr)
	, mIndexBuffer(nullptr)
	, mConstantBuffer(nullptr)
	, mShaderHandler(shaderHandler)
	, mInputLayout(nullptr)
	, mVertexData(nullptr)
	, mVertexCount(0)
	, mIndexCount(0)
	, mModelMat(MatrixMaths::Identity4X4)
{
	// First generate the dome
	GenerateDome(centre, radius, slicesX, slicesY);

	// Now setup the shaders requred for rendering the dome
	SetupShaders();
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

	delete[] mVertexData;
	mVertexData = nullptr;

	delete[] mIndexData;
	mIndexData = nullptr;
}

// ---------------------------------------------------------------- 

void SkyDome::Render(BaseCamera* camera)
{
	// Quick outs
	if (!mVertexShader || !mPixelShader || !mInputLayout ||!camera)
		return;

	// Set the input layout for this render
	mShaderHandler.SetInputLayout(mInputLayout);

	// Set the shaders
	mShaderHandler.SetVertexShader(mVertexShader);
	mShaderHandler.SetPixelShader(mPixelShader);
	
	// Bind the vertex buffer
	unsigned int offset = 0;
	mShaderHandler.BindVertexBuffersToRegisters(0, 1, &mVertexBuffer, &mVertexBufferStride, &offset);

	// Now bind the index buffer
	mShaderHandler.BindIndexBuffersToRegisters(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

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
	if (!mShaderHandler.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP))
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

}

// ---------------------------------------------------------------- 

// X slices is how many steps up the angle must go before reaching straight up
// Y slices is how many steps around the angle must go before reaching back to where it started
void SkyDome::GenerateDome(Vector3D centre, float radius, unsigned int XSlices, unsigned int YSlices)
{
	float theta      = 0.0f; // Rotation angle around the Y axis
	float deltaTheta = TWOPI / float(YSlices);

	float gamma      = 0.0f; // Rotation angle arond the X axis
	float deltaGamma = TWOPI / float(XSlices);

	// First calculate the amount of vertex positions based on the data passed in
	mVertexCount = XSlices * YSlices;

	// Allocate the amount of memory required
	mVertexData = new Vector3D[mVertexCount];

	// Now calculate the actual vertex positions
	unsigned int currentvertex = 0;
	float        sinTheta = 0.0f;
	float        sinGamma = 0.0f;
	float        cosTheta = 0.0f;
    float        cosGamma = 0.0f;

	// Loop around all of the same Y level notches given
	for (unsigned int i = 0; i < YSlices; i++)
	{
		// Reset theta as we have now done a full rotation
		theta = 0.0f;

		// Now loop around all of the same X level notches given
		for (unsigned int j = 0; j < XSlices; j++)
		{
			// Calculate the sin and cos values for what we have now
			sinGamma = sinf(gamma);
			sinTheta = sinf(theta);

			cosTheta = cosf(theta);
			cosGamma = cosf(gamma);

			// Calculate the vertex
			mVertexData[currentvertex] = Vector3D(mRadius * sinGamma * cosTheta, 
												  mRadius * cosGamma,
												  mRadius * sinGamma * sinTheta);

			// Apply the spin of the angle
			theta += deltaTheta;
		}

		// Move the Y level up a notch
		gamma += deltaGamma;
	}

	// Now calculate the indices for rendering

	// Each index will be used twice, as it is used for its current layer, and on the layer below it, appart from the bottom layer and the top most layer, which are only used once each
	mIndexCount = (YSlices * 2) * (XSlices - 1);

	mIndexData = new short[mIndexCount];

	unsigned int currentIndex = 0;
	unsigned int currentValue = 0;

	// Now calculate the indicies for the render
	for (unsigned int YLayer = 0; YLayer < YSlices; YLayer++)
	{
		// To XSlices - 1 because the top most layer would overflow into non-valid indices, and are already included in the layer below the top
		for (unsigned int XNotch = 0; XNotch < XSlices - 1; XNotch++)
		{
			currentValue = (YLayer * XSlices) + XNotch;

			// First put where we are
			mIndexData[currentIndex++] = currentValue;
			
			// Now put the index on the layer above it
			mIndexData[currentIndex++] = currentValue + XSlices;
		}
	}
	
	// Create the vertex buffer now we have the vertex data
	mVertexBufferStride = sizeof(Vector3D);

	if (!mShaderHandler.CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, (D3D11_CPU_ACCESS_FLAG)0, mVertexData, sizeof(Vector3D) * mVertexCount, &mVertexBuffer))
		return;

	if (!mShaderHandler.CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, (D3D11_CPU_ACCESS_FLAG)0, mIndexData, sizeof(short) * mIndexCount, &mIndexBuffer)); 
		return;

	if (!mShaderHandler.BindIndexBuffersToRegisters(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0))
		return;
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