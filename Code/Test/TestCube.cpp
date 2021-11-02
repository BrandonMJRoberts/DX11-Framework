#include "TestCube.h"

#include "../GameScreens/ScreenManager.h"
#include "../Camera/ThirdPersonCamera.h"
#include "../Maths/Commons.h"

// ---------------------------------------------------------------- //

TestCube::TestCube(ShaderHandler& shaderHandler, Vector3D position) 
	: mShaderHandler(shaderHandler)
	, mVertexShader(nullptr)
	, mPixelShader(nullptr)
	, mConstantBuffer(nullptr)
	, mIndexBuffer(nullptr)
	, mVertexBuffer(nullptr)
	, mPosition(position)
{
	// ------------------------------------------------------------------------------------------------------------------------------------- 

	// Matricies
	DirectX::XMStoreFloat4x4(&modelMat,DirectX::XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z));

	// ------------------------------------------------------------------------------------------------------------------------------------- 

	// Shaders
	VertexShaderReturnData returnData    = shaderHandler.CompileVertexShader(L"DX11 Framework.fx", "VS");
	                       mVertexShader = returnData.vertexShader;

	mPixelShader = shaderHandler.CompilePixelShader(L"DX11 Framework.fx", "PS");

	// Now setup the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if(!shaderHandler.SetDeviceInputLayout(returnData.Blob, layout, 2))
		return;

	// Vertex data
	SimpleVertex vertices[] =
	{ 
		// Front four
        { DirectX::XMFLOAT3( -1.0f,  1.0f, -1.0f ), DirectX::XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f ) }, // Top front left
        { DirectX::XMFLOAT3(  1.0f,  1.0f, -1.0f ), DirectX::XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) }, // Top front right
        { DirectX::XMFLOAT3( -1.0f, -1.0f, -1.0f ), DirectX::XMFLOAT4( 0.0f, 1.0f, 1.0f, 1.0f ) }, // Bottom front left
        { DirectX::XMFLOAT3(  1.0f, -1.0f, -1.0f ), DirectX::XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) }, // Bottom front right

        // Back four
        { DirectX::XMFLOAT3( -1.0f,  1.0f, 1.0f ), DirectX::XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) }, // Top Back left
        { DirectX::XMFLOAT3(  1.0f,  1.0f, 1.0f ), DirectX::XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) }, // Top Back right
        { DirectX::XMFLOAT3( -1.0f, -1.0f, 1.0f ), DirectX::XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f ) }, // Bottom back left
        { DirectX::XMFLOAT3(  1.0f, -1.0f, 1.0f ), DirectX::XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) }, // Bottom back right
    };

	// Now for the vertex and index buffers
	if (!shaderHandler.CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, (D3D11_CPU_ACCESS_FLAG)0, vertices, sizeof(SimpleVertex) * 8, &mVertexBuffer))
		return;

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	if (!shaderHandler.BindVertexBuffersToRegisters(0, 1, &mVertexBuffer, &stride, &offset))
		return;

	// ------------------------------------------------------------------------------------------------------------------------------------- 

	WORD indicies[] = 
	{
		// Front
        0,1,2,
        2,1,3,

        // Back
        5,4,6,
        7,5,6,

        // Top
        4,5,0,
        5,1,0,

        // Bottom
        2,3,6,
        3,7,6,

        // Left
        4,0,6,
        0,2,6,

        // Right
        1,5,3,
        5,7,3
	};

	if (!shaderHandler.CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, (D3D11_CPU_ACCESS_FLAG)0, indicies, sizeof(WORD) * 36, &mIndexBuffer))
		return;

	if (!shaderHandler.BindIndexBuffersToRegisters(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0))
		return;

	// ------------------------------------------------------------------------------------------------------------------------------------- 

	// Now set the primitive topology
	if (!shaderHandler.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST))
		return;

	// ------------------------------------------------------------------------------------------------------------------------------------- 

	// Now for the constant buffer
	if (!shaderHandler.CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, (D3D11_CPU_ACCESS_FLAG)0, nullptr, sizeof(ConstantBuffer), &mConstantBuffer))
		return;

	// ------------------------------------------------------------------------------------------------------------------------------------- 
}

// ---------------------------------------------------------------- //

TestCube::~TestCube()
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

	if (mConstantBuffer)
	{
		mConstantBuffer->Release();
		mConstantBuffer = nullptr;
	}

	if (mIndexBuffer)
	{
		mIndexBuffer->Release();
		mIndexBuffer = nullptr;
	}

	if (mVertexBuffer)
	{
		mVertexBuffer->Release();
		mVertexBuffer = nullptr;
	}
}

// ---------------------------------------------------------------- //

void TestCube::Render(BaseCamera* camera)
{
	if (!camera)
		return;

	// Get the local stored world matrix for this object
	DirectX::XMMATRIX world      = DirectX::XMLoadFloat4x4(&modelMat);

	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(world);

	// Get the view and projection matrixes from the camera and transpose them before passing into the shader
	DirectX::XMMATRIX   view;
	DirectX::XMFLOAT4X4 view4X4  = camera->GetViewMatrix();
					    view     = DirectX::XMLoadFloat4x4(&view4X4);
	                    cb.mView = DirectX::XMMatrixTranspose(view);

	DirectX::XMMATRIX projection;
	DirectX::XMFLOAT4X4 persp        = camera->GetPerspectiveMatrix();
					  projection     = DirectX::XMLoadFloat4x4(&persp);
					  cb.mProjection = DirectX::XMMatrixTranspose(projection);

	mShaderHandler.UpdateSubresource(mConstantBuffer, 0, nullptr, &cb, 0, 0);

	// Set the shaders
	mShaderHandler.SetVertexShader(mVertexShader);
	mShaderHandler.SetVertexShaderConstantBufferData(0, 1, &mConstantBuffer);

	mShaderHandler.SetPixelShader(mPixelShader);
	mShaderHandler.SetPixelShaderConstantBufferData(0, 1, &mConstantBuffer);

	// Draw
	mShaderHandler.DrawIndexed(36, 0, 0);
}

// ---------------------------------------------------------------- //

void TestCube::Update(const float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);

	// Make sure that the cube's position is always where it is internally stored
	DirectX::XMStoreFloat4x4(&modelMat, DirectX::XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z));
}

// ---------------------------------------------------------------- //

void TestCube::move(const float deltaTime)
{
	mPosition.y += 0.1f * deltaTime;
}

// ---------------------------------------------------------------- //