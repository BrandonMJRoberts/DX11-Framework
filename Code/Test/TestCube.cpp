#include "TestCube.h"

#include "../GameScreens/ScreenManager.h"

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
	DirectX::XMStoreFloat4x4(&modelMat, DirectX::XMMatrixAffineTransformation(DirectX::XMVectorSet(1.0f, 1.0f, 1.0, 1.0f), DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), DirectX::XMVectorSet(mPosition.x, mPosition.y, mPosition.z, 0.0f)));

	// Would normally be through a camera but this is for a test
	DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 0.0f, -6.0f, 0.0f);
	DirectX::XMVECTOR at  = DirectX::XMVectorSet(0.0f, 0.0f,  0.0f, 0.0f);
	DirectX::XMVECTOR up  = DirectX::XMVectorSet(0.0f, 1.0f,  0.0f, 0.0f);

	DirectX::XMStoreFloat4x4(&viewMat, DirectX::XMMatrixLookAtLH(eye, at, up));

	DirectX::XMStoreFloat4x4(&projectionMat, DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, GameScreenManager::ScreenWidth / (float)GameScreenManager::ScreenHeight, 0.01f, 100.0f));

	// ------------------------------------------------------------------------------------------------------------------------------------- 

	// Shaders
	VertexShaderReturnData returnData    = shaderHandler.CompileVertexShader(L"DX11 Framework.fx", "VS");
	                       mVertexShader = returnData.vertexShader;

	mPixelShader = shaderHandler.CompilePixelShader(L"DX11 Framework.fx", "PS");

	// Now setup the input layout
	if(!shaderHandler.SetDeviceInputLayout(returnData.Blob))
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
}

// ---------------------------------------------------------------- //

void TestCube::Render()
{
	DirectX::XMMATRIX world      = DirectX::XMLoadFloat4x4(&modelMat);
	DirectX::XMMATRIX view       = DirectX::XMLoadFloat4x4(&viewMat);
	DirectX::XMMATRIX projection = DirectX::XMLoadFloat4x4(&projectionMat);

	ConstantBuffer cb;
	cb.mWorld      = XMMatrixTranspose(world);
	cb.mView       = XMMatrixTranspose(view);
	cb.mProjection = XMMatrixTranspose(projection);

	mShaderHandler.UpdateSubresource(mConstantBuffer, 0, nullptr, &cb, 0, 0);

	// Set the shaders
	mShaderHandler.SetVertexShader(mVertexShader);
	mShaderHandler.SetVertexShaderConstantBufferData(0, 1, &mConstantBuffer);

	mShaderHandler.SetPixelShader(mPixelShader);
	mShaderHandler.SetPixelShaderConstantBufferData(0, 1, &mConstantBuffer);

	mShaderHandler.DrawIndexed(36, 0, 0);
}

// ---------------------------------------------------------------- //

void TestCube::Update(const float deltaTime)
{
	//DirectX::XMStoreFloat4x4(&modelMat, DirectX::XMMatrixAffineTransformation(DirectX::XMVectorSet(10.0f, 10.0f, 10.0f, 0.0f), DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)));
	//DirectX::XMStoreFloat4x4(&modelMat, DirectX::XMMatrixRotationZ(deltaTime) * DirectX::XMMatrixRotationY(deltaTime));
}

// ---------------------------------------------------------------- //