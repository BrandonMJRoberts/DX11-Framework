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

    , mCurrentBlendState(nullptr)
    , mCurrentBlendFactor{ 1.0f, 1.0f, 1.0f, 1.0f }
    
    , mPriorBlendState(nullptr)
    , mPriorBlendFactor{ 1.0f, 1.0f, 1.0f, 1.0f }

    , mDefaultBlendState(nullptr)
{
    // Create the default blend state and set the prior and current states to it
    CreateBlendState(&mDefaultBlendState, false, D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD, D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL);

    mPriorBlendState   = mDefaultBlendState;
    mCurrentBlendState = mDefaultBlendState;
}

// ------------------------------------------------------------------------------------------ //

ShaderHandler::~ShaderHandler()
{
    mDeviceHandle      = nullptr;
    mDeviceContext     = nullptr;

    if (mDefaultBlendState)
    {
        if (mDefaultBlendState == mPriorBlendState)
            mPriorBlendState = nullptr;

        if (mDefaultBlendState == mCurrentBlendState)
            mCurrentBlendState = nullptr;

        mDefaultBlendState->Release();
        mDefaultBlendState = nullptr;
    }

    if (mPriorBlendState)
    {
        if (mPriorBlendState == mCurrentBlendState)
            mCurrentBlendState = nullptr;

        mPriorBlendState->Release();
        mPriorBlendState = nullptr;
    }

    if (mCurrentBlendState)
    {
        mCurrentBlendState->Release();
        mCurrentBlendState = nullptr;
    }
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
bool ShaderHandler::SetDeviceInputLayout(ID3DBlob* vertexShaderBlob, D3D11_INPUT_ELEMENT_DESC layout[], unsigned int numberOfElementsInArray, ID3D11InputLayout** vertexLayout, bool clearBlob)
{
    // Create the input layout
    HRESULT hr;
    hr = mDeviceHandle->CreateInputLayout(layout, numberOfElementsInArray, vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), vertexLayout);
    if (FAILED(hr))
    {
        if (clearBlob)
        {
            vertexShaderBlob->Release();
        }

        return false;
    }

    // Free the data we dont need anymore
    if (clearBlob)
    {
        vertexShaderBlob->Release();
        vertexShaderBlob = nullptr;
    }

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

bool ShaderHandler::Draw(unsigned int vertexCount, unsigned int startLocation)
{
    if (!mDeviceContext)
        return false;

    mDeviceContext->Draw(vertexCount, startLocation);

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

bool ShaderHandler::CreateTexture3D(D3D11_TEXTURE3D_DESC* description, const D3D11_SUBRESOURCE_DATA* initialData, ID3D11Texture3D** texture)
{
    if (!mDeviceHandle)
        return false;

    HRESULT hr;

    hr = mDeviceHandle->CreateTexture3D(description, initialData, texture);

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

bool ShaderHandler::DrawInstanced(unsigned int vertexCountPerInstance, unsigned int numberOfInstancesToDraw, unsigned int startVertexLocation, unsigned int startInstanceLocation)
{
    if (!mDeviceContext)
        return false;

    mDeviceContext->DrawInstanced(vertexCountPerInstance, numberOfInstancesToDraw, startVertexLocation, startInstanceLocation);

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
    if (!mDeviceContext)
        return;

    mDeviceContext->PSSetSamplers(startSlot, count, samplerState);
}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::SetViewport(float width, float height, float minDepth, float maxDepth, unsigned int topLeftX, unsigned int topLeftY)
{
    if (!mDeviceContext)
        return;

    D3D11_VIEWPORT vp;
    vp.Width    = width;
    vp.Height   = height;
    vp.MinDepth = minDepth;
    vp.MaxDepth = maxDepth;
    vp.TopLeftX = (FLOAT)topLeftX;
    vp.TopLeftY = (FLOAT)topLeftY;

    // Apply the viewport change
    mDeviceContext->RSSetViewports(1, &vp);
}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::UnbindTextureFromShaderIndex(unsigned int startSlot, unsigned int numberOfViews)
{
    if (!mDeviceContext)
        return;

    ID3D11ShaderResourceView* shaderViewEmpty = nullptr;

    mDeviceContext->PSSetShaderResources(startSlot, numberOfViews, &shaderViewEmpty);
}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::CreateRasterizerState(ID3D11RasterizerState** state, 
                                          D3D11_FILL_MODE         fillMode, 
                                          D3D11_CULL_MODE         cullMode, 
                                          bool                    windingCounterClockwise, 
                                          int                     depthBias,          
                                          float                   depthBiasClamp, 
                                          float                   slopeScaledDepthBias, 
                                          bool                    depthClipEnabled, 
                                          bool                    scissorEnabled, 
                                          bool                    multiSampleEnabled, 
                                          bool                    antialiasedLineEnabled)
{
    if (!mDeviceContext)
        return;

    D3D11_RASTERIZER_DESC desc;
    desc.CullMode              = cullMode;
    desc.FillMode              = fillMode;
    desc.FrontCounterClockwise = windingCounterClockwise;
    desc.DepthBias             = depthBias;
    desc.DepthBiasClamp        = depthBiasClamp;
    desc.AntialiasedLineEnable = antialiasedLineEnabled;
    desc.SlopeScaledDepthBias  = slopeScaledDepthBias;
    desc.DepthClipEnable       = depthClipEnabled;
    desc.ScissorEnable         = scissorEnabled;
    desc.MultisampleEnable     = multiSampleEnabled;

    HRESULT hr;

    hr = mDeviceHandle->CreateRasterizerState(&desc, state);
}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::BindRasterizerState(ID3D11RasterizerState* state)
{
    if (!mDeviceContext)
        return;

    mDeviceContext->RSSetState(state);
}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::DispatchComputeShader(unsigned int threadGroupCountX, unsigned int threadGroupCountY, unsigned int threadGroupCountZ)
{
    if (!mDeviceContext)
    {
        return;
    }

    mDeviceContext->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

// ------------------------------------------------------------------------------------------ //

bool ShaderHandler::CreateUnorderedAccessView(ID3D11Resource* resource, D3D11_UNORDERED_ACCESS_VIEW_DESC* desc, ID3D11UnorderedAccessView** viewReturn)
{
    if (!mDeviceHandle)
    {
        return false;
    }

    HRESULT hr;
    hr = mDeviceHandle->CreateUnorderedAccessView(resource, desc, viewReturn);

    if (FAILED(hr))
        return false;

    return true;
}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::BindComputeShader(ID3D11ComputeShader* shader)
{
    if (!mDeviceContext)
    {
        return;
    }

    mDeviceContext->CSSetShader(shader, nullptr, 0);
}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::BindShaderResouceToComputeShader(unsigned int startSlot, unsigned int numberOfViews, ID3D11ShaderResourceView* const* views)
{
    if (!mDeviceContext)
    {
        return;
    }

    mDeviceContext->CSSetShaderResources(startSlot, numberOfViews, views);
}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::BindUnorderedAccessViewToComputeShader(unsigned int startSlot, unsigned int numberToBind, ID3D11UnorderedAccessView* const* accessViews, const unsigned int* UAVInitialCounts)
{
    if (!mDeviceContext)
    {
        return;
    }

    mDeviceContext->CSSetUnorderedAccessViews(startSlot, numberToBind, accessViews, UAVInitialCounts);
}

// ------------------------------------------------------------------------------------------ //

bool ShaderHandler::CreateComputeShader(LPCWSTR fileName, LPCSTR entryPoint, ID3D11ComputeShader** computeShaderReturn)
{
    if (!mDeviceHandle)
    {
        return false;
    }

    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob  = nullptr;

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
    #if defined( DEBUG ) || defined( _DEBUG )
        flags |= D3DCOMPILE_DEBUG;
    #endif

    LPCSTR profile = (mDeviceHandle->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";

    HRESULT hr = D3DCompileFromFile(fileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, profile, flags, 0, &shaderBlob, &errorBlob);

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            const char* errorMessage = (const char*)errorBlob->GetBufferPointer();

            errorBlob->Release();
        }

        if (shaderBlob)
            shaderBlob->Release();

        return false;
    }

    hr = mDeviceHandle->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, computeShaderReturn);

    if (FAILED(hr))
        return false;

    return true;
}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::UnbindUnorderedAccessViewFromComputeShader(unsigned int startSlot, unsigned int count)
{
    if (!mDeviceContext)
        return;

    ID3D11UnorderedAccessView* nullUAV[1] = { 0 };

    mDeviceContext->CSSetUnorderedAccessViews(startSlot, count, nullUAV, 0);
}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::CreateBlendState(const D3D11_BLEND_DESC* desc, ID3D11BlendState** returnBlendState)
{
    if (!mDeviceHandle)
        return;

    mDeviceHandle->CreateBlendState(desc, returnBlendState);
}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::CreateBlendState(ID3D11BlendState** returnBlendState, 
                                     bool               blendEnabled,  
                                     D3D11_BLEND        srcBlend,  
                                     D3D11_BLEND        destBlend, 
                                     D3D11_BLEND_OP     blendOp,
                                     D3D11_BLEND        srcBlendAlpha,
                                     D3D11_BLEND        destBlendAlpha,
                                     D3D11_BLEND_OP     blendOpAlpha,
                                     UINT8              renderTargetWriteMask)
{
    if (!mDeviceHandle)
        return;

    D3D11_BLEND_DESC desc;
    desc.AlphaToCoverageEnable  = false;
    desc.IndependentBlendEnable = false;

    desc.RenderTarget[0].BlendEnable           = blendEnabled;
    desc.RenderTarget[0].SrcBlend              = srcBlend;
    desc.RenderTarget[0].DestBlend             = destBlend;
    desc.RenderTarget[0].BlendOp               = blendOp;
    desc.RenderTarget[0].SrcBlendAlpha         = srcBlendAlpha;
    desc.RenderTarget[0].DestBlendAlpha        = destBlendAlpha;
    desc.RenderTarget[0].BlendOpAlpha          = blendOpAlpha;
    desc.RenderTarget[0].RenderTargetWriteMask = renderTargetWriteMask; 

    HRESULT hr;

    hr = mDeviceHandle->CreateBlendState(&desc, returnBlendState);

    if (FAILED(hr))
        return;
}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::BindBlendState(ID3D11BlendState* blendState, float blendFactor[4], UINT sampleMask)
{
    if (!mDeviceContext)
        return;

    mDeviceContext->OMSetBlendState(blendState, blendFactor, sampleMask);

    // Store the old state as the prior state in case we want to swap back to it
    mPriorBlendState     = mCurrentBlendState;
    mPriorBlendFactor[0] = mCurrentBlendFactor[0];
    mPriorBlendFactor[1] = mCurrentBlendFactor[1];
    mPriorBlendFactor[2] = mCurrentBlendFactor[2];
    mPriorBlendFactor[3] = mCurrentBlendFactor[3];

    // Store this as the current blend state
    mCurrentBlendState     = blendState;
    mCurrentBlendFactor[0] = blendFactor[0];
    mCurrentBlendFactor[1] = blendFactor[1];
    mCurrentBlendFactor[2] = blendFactor[2];
    mCurrentBlendFactor[3] = blendFactor[3];
}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::BindPriorBlendState()
{
    if (!mDeviceContext)
        return;

    // Bind the prior state
    mDeviceContext->OMSetBlendState(mPriorBlendState, mPriorBlendFactor, 0xffffffff);

    // Swap the prior and current states
    float             placeHolderFactor[4]  = { mPriorBlendFactor[0], mPriorBlendFactor[1], mPriorBlendFactor[2], mPriorBlendFactor[3] };
    ID3D11BlendState* placeholderBlendState = mPriorBlendState;

    // Swap them over
    mPriorBlendState     = mCurrentBlendState;
    mPriorBlendFactor[0] = mCurrentBlendFactor[0];
    mPriorBlendFactor[1] = mCurrentBlendFactor[1];
    mPriorBlendFactor[2] = mCurrentBlendFactor[2];
    mPriorBlendFactor[3] = mCurrentBlendFactor[3];

    mCurrentBlendState     = placeholderBlendState;
    mCurrentBlendFactor[0] = placeHolderFactor[0];
    mCurrentBlendFactor[1] = placeHolderFactor[1];
    mCurrentBlendFactor[2] = placeHolderFactor[2];
    mCurrentBlendFactor[3] = placeHolderFactor[3];
}

// ------------------------------------------------------------------------------------------ //

void ShaderHandler::BindDefaultBlendState()
{
    if (!mDeviceContext)
        return;

    float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    mDeviceContext->OMSetBlendState(mDefaultBlendState, blendFactor, 0xffffffff);
}

// ------------------------------------------------------------------------------------------ //