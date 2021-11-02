#include "ShaderHandler.h"

#include <d3dcompiler.h>
#include <iostream>

// ------------------------------------------------------------------------------------------ //

ShaderHandler::ShaderHandler(ID3D11Device* deviceHandle, ID3D11DeviceContext* deviceContextHandle, ID3D11RenderTargetView* defaultBackBuffer, ID3D11DepthStencilView* defaultDepthBuffer)
    : mDeviceHandle(deviceHandle)
    , mDeviceContext(deviceContextHandle)
    , mDefaultBackBuffer(defaultBackBuffer)
    , mDefaultDepthStencilBuffer(defaultDepthBuffer)

    , mCurrentRenderBuffer(defaultBackBuffer)
    , mCurrentDepthStencilBuffer(defaultDepthBuffer)

    , mPriorRenderBuffer(defaultBackBuffer)
    , mPriorDepthStencilBuffer(defaultDepthBuffer)
{
    
}

// ------------------------------------------------------------------------------------------ //

ShaderHandler::~ShaderHandler()
{
    mDeviceHandle      = nullptr;
    mDeviceContext     = nullptr;
}

// ------------------------------------------------------------------------------------------ //

VertexShaderReturnData ShaderHandler::CompileVertexShader(WCHAR* filePathToOverallShader, LPCSTR nameOfVertexMainFunction)
{
    VertexShaderReturnData returnData{nullptr, nullptr};

    // Compile the shader 
    ID3DBlob* pVSBlob = nullptr;              

    // Check that the overall shader has been loaded
    if (!CompileShaderFromFile(filePathToOverallShader, nameOfVertexMainFunction, "vs_4_0", &pVSBlob))
    {
        // Output the error
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);

        // return back nothing
        return returnData;
    }

	// Create the vertex shader
    ID3D11VertexShader* vertexShader;
    HRESULT hr;
    hr = mDeviceHandle->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &vertexShader);
	if(FAILED(hr))
    {	
        // Clean up the data
		pVSBlob->Release();

        // Return back nothing
        return returnData;
	}

    // Return the successful compilation of the shader
    returnData.Blob         = pVSBlob;
    returnData.vertexShader = vertexShader;

    return returnData;
}

// ------------------------------------------------------------------------------------------ //

ID3D11PixelShader* ShaderHandler::CompilePixelShader(WCHAR* filePathToOverallShader, LPCSTR nameOfPixelMainFunction)
{
    // Compile the shader 
    ID3DBlob* pPSBlob = nullptr;

    // Check that the overall shader has been loaded
    if (!CompileShaderFromFile(filePathToOverallShader, nameOfPixelMainFunction, "ps_4_0", &pPSBlob))
    {
        // Output the error
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);

        // return back nothing
        return nullptr;
    }

    // Create the vertex shader
    ID3D11PixelShader* pixelShader;
    HRESULT hr;
            hr = mDeviceHandle->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pixelShader);
    if (FAILED(hr))
    {
        // Clean up the data
        pPSBlob->Release();

        // Return back nothing
        return nullptr;
    }

    // Return the successful compilation of the shader
    return pixelShader;
}

// ------------------------------------------------------------------------------------------ //

// Setting how the device will be accessing from shader buffers - when 
bool ShaderHandler::SetDeviceInputLayout(ID3DBlob* vertexShaderBlob, D3D11_INPUT_ELEMENT_DESC layout[], unsigned int numberOfElementsInArray, ID3D11InputLayout** vertexLayout)
{
    // Create the input layout
    HRESULT hr;
    hr = mDeviceHandle->CreateInputLayout(layout, numberOfElementsInArray, vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), vertexLayout);
    if (FAILED(hr))
    {
        vertexShaderBlob->Release();
        return false;
    }

    // Free the data we dont need anymore
    vertexShaderBlob->Release();
    vertexShaderBlob = nullptr;

    return true;
}

void ShaderHandler::SetInputLayout(ID3D11InputLayout* vertexLayout)
{
    if (!mDeviceContext)
        return;

    // Set the input layout
    mDeviceContext->IASetInputLayout(vertexLayout);
}

// ------------------------------------------------------------------------------------------ //

bool ShaderHandler::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
    
    // Compile the shader from the file path given
    ID3DBlob* pErrorBlob;
    HRESULT hr;
            hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
    if (FAILED(hr))
    {
        // Output the error message
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        // Memory cleanup
        if (pErrorBlob) 
            pErrorBlob->Release();

        // Return that the function failed
        return false;
    }

    // Release the memory we no longer need as the shader has been compiled
    if (pErrorBlob) 
        pErrorBlob->Release();

    // Return the success
    return true;
}

// ------------------------------------------------------------------------------------------ //

bool ShaderHandler::CreateBuffer(D3D11_USAGE usageType, D3D11_BIND_FLAG bindFlags, D3D11_CPU_ACCESS_FLAG  CPUAccessFlag, void* bufferData, unsigned int bytesInBuffer, ID3D11Buffer** returnBuffer)
{
    // Quick out
    if (!mDeviceHandle)
        return false;

    HRESULT hr = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = bufferData;

    // Setup the buffer description
    D3D11_BUFFER_DESC bufferDescription;
    bufferDescription.Usage          = usageType;
    bufferDescription.ByteWidth      = bytesInBuffer;
    bufferDescription.BindFlags      = bindFlags;
    bufferDescription.CPUAccessFlags = CPUAccessFlag;
    bufferDescription.MiscFlags      = 0;

    if (bufferData)
    {
        // Create the buffer
        hr = mDeviceHandle->CreateBuffer(&bufferDescription, &InitData, returnBuffer);
    }
    else
    {
        hr = mDeviceHandle->CreateBuffer(&bufferDescription, nullptr, returnBuffer);
    }

    if (FAILED(hr))
    {
        if(returnBuffer && *returnBuffer)
            (*returnBuffer)->Release();

        std::cout << "Failed to create the buffer!" << std::endl;

        return false;
    }

    return true;
}

// ------------------------------------------------------------------------------------------ //

bool ShaderHandler::BindVertexBuffersToRegisters(unsigned int startSlot, unsigned int numberOfBuffers, ID3D11Buffer* const* buffers, const unsigned int* strides, const unsigned int* offsets)
{
    if (!mDeviceContext)
        return false;

    // Set the vertex buffers to the registers
    mDeviceContext->IASetVertexBuffers(startSlot, numberOfBuffers, buffers, strides, offsets);

    return true;
}

// ------------------------------------------------------------------------------------------ //

bool ShaderHandler::BindIndexBuffersToRegisters(ID3D11Buffer* indexBuffer, DXGI_FORMAT format, unsigned int offset)
{
    if (!mDeviceContext)
        return false;

    // Set the index buffer to the register
    mDeviceContext->IASetIndexBuffer(indexBuffer, format, offset);

    return true;
}

// ------------------------------------------------------------------------------------------ //

bool ShaderHandler::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
{
    if (!mDeviceContext)
        return false;

    mDeviceContext->IASetPrimitiveTopology(topology);

    return true;
}

// ------------------------------------------------------------------------------------------ //

bool ShaderHandler::SetVertexShader(ID3D11VertexShader* vertexShader)
{
    if (!mDeviceContext)
        return false;

    mDeviceContext->VSSetShader(vertexShader, nullptr, 0);

    return true;
}

// ------------------------------------------------------------------------------------------ //

bool ShaderHandler::SetPixelShader(ID3D11PixelShader* pixelShader)
{
    if (!mDeviceContext)
        return false;

    mDeviceContext->PSSetShader(pixelShader, nullptr, 0);

    return true;
}

// ------------------------------------------------------------------------------------------ //

bool ShaderHandler::SetVertexShaderConstantBufferData(unsigned int startSlot, unsigned int numberOfbuffers, ID3D11Buffer* const* buffers)
{
    if (!mDeviceContext)
        return false;

    mDeviceContext->VSSetConstantBuffers(startSlot, numberOfbuffers, buffers);

    return true;
}

// ------------------------------------------------------------------------------------------ //

bool ShaderHandler::SetPixelShaderConstantBufferData(unsigned int startSlot, unsigned int numberOfbuffers, ID3D11Buffer* const* buffers)
{
    if (!mDeviceContext)
        return false;

    mDeviceContext->PSSetConstantBuffers(startSlot, numberOfbuffers, buffers);

    return true;
}

// ------------------------------------------------------------------------------------------ //

bool ShaderHandler::DrawIndexed(unsigned int numberOfIndicies, unsigned int startIndexLocation, int baseVertexLocation)
{
    if (!mDeviceContext)
        return false;

    mDeviceContext->DrawIndexed(numberOfIndicies, startIndexLocation, baseVertexLocation);

    return true;
}

// ------------------------------------------------------------------------------------------ //

bool ShaderHandler::UpdateSubresource(ID3D11Resource* destResource, unsigned int destSubResource, const D3D11_BOX* destBox, const void* sourceData, unsigned int sourceRowPitch, unsigned int sourceDepthPitch)
{
    if (!mDeviceContext)
        return false;

    mDeviceContext->UpdateSubresource(destResource, destSubResource, destBox, sourceData, sourceRowPitch, sourceDepthPitch);

    return true;
}

// ------------------------------------------------------------------------------------------ //

bool ShaderHandler::CreateTexture2D(D3D11_TEXTURE2D_DESC* description, const D3D11_SUBRESOURCE_DATA* initialData, ID3D11Texture2D** texture)
{
    if (!mDeviceHandle)
        return false;

    HRESULT hr;

    hr = mDeviceHandle->CreateTexture2D(description, initialData, texture);

    return (!FAILED(hr));
}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::SetRenderTargets(unsigned int numberToBind, ID3D11RenderTargetView* const* renderTargetViewsToBind, ID3D11DepthStencilView* depthStencilViewToBind)
{
    // Check the context exists
    if (!mDeviceContext)
        return;

    // Check we are not trying to bind too many 
    if (numberToBind > D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)
        return;

    mPriorRenderBuffer        = mCurrentRenderBuffer;
    mPriorDepthStencilBuffer  = mCurrentDepthStencilBuffer;

    mCurrentRenderBuffer       = renderTargetViewsToBind[0];
    mCurrentDepthStencilBuffer = depthStencilViewToBind;

    // Bind the render targets
    mDeviceContext->OMSetRenderTargets(numberToBind, renderTargetViewsToBind, depthStencilViewToBind);
}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::SetDefaultRenderTarget()
{
    // Check the context exists
    if (!mDeviceContext)
        return;

    mPriorRenderBuffer         = mCurrentRenderBuffer;
    mPriorDepthStencilBuffer   = mCurrentDepthStencilBuffer;

    mCurrentRenderBuffer       = mDefaultBackBuffer;
    mCurrentDepthStencilBuffer = mDefaultDepthStencilBuffer;

    // Re-bind our stored default back buffer and depth buffer
    mDeviceContext->OMSetRenderTargets(1, &mDefaultBackBuffer, mDefaultDepthStencilBuffer);
}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::SetPriorRenderTarget()
{
    if (!mDeviceContext)
        return;

    mDeviceContext->OMSetRenderTargets(1, &mPriorRenderBuffer, mPriorDepthStencilBuffer);

    ID3D11RenderTargetView* tempRenderBuffer;
    ID3D11DepthStencilView* tempDepthStencilBuffer;

    tempRenderBuffer           = mCurrentRenderBuffer;
    tempDepthStencilBuffer     = mCurrentDepthStencilBuffer;

    mCurrentRenderBuffer       = mPriorRenderBuffer;
    mCurrentDepthStencilBuffer = mPriorDepthStencilBuffer;

    mPriorRenderBuffer         = tempRenderBuffer;
    mPriorDepthStencilBuffer   = tempDepthStencilBuffer;
}

// ------------------------------------------------------------------------------------------ //

bool ShaderHandler::DrawInstanced(unsigned int numberOfInstancesToDraw)
{
    return true;
}

// ------------------------------------------------------------------------------------------ //

bool ShaderHandler::CreateRenderTargetView(ID3D11Resource* renderTargetTexture, const D3D11_RENDER_TARGET_VIEW_DESC* renderTargetViewDesc, ID3D11RenderTargetView** renderTargetView)
{
    if (!mDeviceHandle)
        return false;

    mDeviceHandle->CreateRenderTargetView(renderTargetTexture, renderTargetViewDesc, renderTargetView);

    return true;
}

// ------------------------------------------------------------------------------------------ //

bool ShaderHandler::CreateShaderResourceView(ID3D11Resource* shaderResourceTexture, const D3D11_SHADER_RESOURCE_VIEW_DESC* shaderResourceViewDesc, ID3D11ShaderResourceView** shaderResourceView)
{
    if (!mDeviceHandle)
        return false;

    HRESULT hr;

    // Create the resource view
    hr = mDeviceHandle->CreateShaderResourceView(shaderResourceTexture, shaderResourceViewDesc, shaderResourceView);

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::ClearRenderTargetView(ID3D11RenderTargetView* renderTargetToClear, const float colour[4])
{
    if (!mDeviceContext || !renderTargetToClear)
        return;

    mDeviceContext->ClearRenderTargetView(renderTargetToClear, colour);
}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::ClearDepthStencilView(ID3D11DepthStencilView* depthStencilViewToClear, unsigned int clearFlags, float depth, UINT8 stencil)
{
    if (!mDeviceContext || !depthStencilViewToClear)
        return;

    mDeviceContext->ClearDepthStencilView(depthStencilViewToClear, clearFlags, depth, stencil);
}

// ------------------------------------------------------------------------------------------ //

bool ShaderHandler::CreateDepthStencilView(ID3D11Resource* depthStencilTexture, const D3D11_DEPTH_STENCIL_VIEW_DESC* description, ID3D11DepthStencilView** depthStencilView)
{
    if (!mDeviceHandle)
        return false;

    HRESULT hr;

    hr = mDeviceHandle->CreateDepthStencilView(depthStencilTexture, description, depthStencilView);

    return (!FAILED(hr));
}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::SetNoDepthStencilBuffer()
{
    if (!mDeviceContext)
        return;

    mDeviceContext->OMSetRenderTargets(1, &mDefaultBackBuffer, nullptr);
}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::DisableColourBufferRendering()
{
   
}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::EnableColourBufferRendering()
{

}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::DisableDepthBufferingRendering()
{

}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::EnableDepthBufferingRendering()
{

}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::DisableStencilBufferingRendering()
{

}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::EnableStencilBufferingRendering()
{

}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::DisableDepthStencilBufferRendering()
{

}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::EnableDepthStencilBufferRendering()
{

}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::BindTextureToShaders(unsigned int startSlot, unsigned int numberOfViews, ID3D11ShaderResourceView** shaderResourceViews)
{
    if (!mDeviceContext)
        return;

    mDeviceContext->PSSetShaderResources(startSlot, numberOfViews, shaderResourceViews);
}

// ------------------------------------------------------------------------------------------ //

bool ShaderHandler::CreateSamplerState(D3D11_SAMPLER_DESC* samplerDescription, ID3D11SamplerState** samplerState)
{
    if (!mDeviceHandle)
        return false;

    HRESULT hr;

    hr = mDeviceHandle->CreateSamplerState(samplerDescription, samplerState);

    if (FAILED(hr))
        return false;

    return true;
}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::BindSamplerState(unsigned int startSlot, unsigned int count, ID3D11SamplerState* const* samplerState)
{
    if (!mDeviceHandle)
        return;

    mDeviceContext->PSSetSamplers(startSlot, count, samplerState);
}

// ------------------------------------------------------------------------------------------ //