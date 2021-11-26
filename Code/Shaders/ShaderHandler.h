#ifndef _SHADER_HANDLER_H_
#define _SHADER_HANDLER_H_

#include <windows.h>

#include <d3d11_1.h>
#include <d3d9types.h>

#include <directxmath.h>
#include <string>

// ----------------------------------------------------------------------------------------------- /

struct VertexShaderReturnData
{
	ID3D11VertexShader* vertexShader;
	ID3DBlob*			Blob;
};

// ----------------------------------------------------------------------------------------------- /

class ShaderHandler final
{
public:
	ShaderHandler(ID3D11Device* deviceHandle, ID3D11DeviceContext* deviceContextHandle, ID3D11RenderTargetView* defaultBackBuffer, ID3D11DepthStencilView* defaultDepthBuffer);
	~ShaderHandler();

	// -----------------------------------------

	// Create the actual shaders of each type
	VertexShaderReturnData CompileVertexShader(WCHAR* filePathToOverallShader, LPCSTR nameOfVertexMainFunction);
	ID3D11PixelShader*     CompilePixelShader(WCHAR* filePathToOverallShader, LPCSTR nameOfPixelMainFunction);

	// Current Shader Setter
	bool				   SetVertexShader(ID3D11VertexShader* vertexShader);
	bool				   SetPixelShader(ID3D11PixelShader* pixelShader);

	// -----------------------------------------

	// Setting how the device will be accessing from shader buffers
	bool SetDeviceInputLayout(ID3DBlob* vertexShaderBlob, D3D11_INPUT_ELEMENT_DESC layout[], unsigned int numberOfElementsInArray, ID3D11InputLayout** vertexLayout, bool clearBlob = true);
	void SetInputLayout(ID3D11InputLayout* vertexLayout);

	bool CreateSamplerState(D3D11_SAMPLER_DESC* samplerDescription, ID3D11SamplerState** samplerState);
	void BindSamplerState(unsigned int startSlot, unsigned int count, ID3D11SamplerState* const* samplerState);

	// -----------------------------------------

	// Buffer creation
	bool CreateBuffer(D3D11_USAGE usageType, D3D11_BIND_FLAG bindFlags, D3D11_CPU_ACCESS_FLAG  CPUAccessFlag, void* bufferData, unsigned int bytesInBuffer, ID3D11Buffer** returnBuffer);

	// Buffer binding functionality
	bool BindVertexBuffersToRegisters(unsigned int startSlot, unsigned int numberOfBuffers, ID3D11Buffer* const* buffers, const unsigned int* strides, const unsigned int* offsets);
	bool BindIndexBuffersToRegisters(ID3D11Buffer* indexBuffer, DXGI_FORMAT format, unsigned int offset);

	// Constant buffer setter
	bool SetVertexShaderConstantBufferData(unsigned int startSlot, unsigned int numberOfbuffers, ID3D11Buffer* const* buffers);
	bool SetPixelShaderConstantBufferData(unsigned int startSlot, unsigned int numberOfbuffers, ID3D11Buffer* const* buffers);

	bool UpdateSubresource(ID3D11Resource* destResource, unsigned int destSubResource, const D3D11_BOX* destBox, const void* sourceData, unsigned int sourceRowPitch, unsigned int sourceDepthPitch);

	void UnbindTextureFromShaderIndex(unsigned int startSlot, unsigned int numberOfViews);
	void BindTextureToShaders(unsigned int startSlot, unsigned int numberOfViews, ID3D11ShaderResourceView** shaderResourceViews);

	// -----------------------------------------

	// Data topology type setter
	bool SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);

	// -----------------------------------------

	// Texture creation
	bool CreateTexture2D(D3D11_TEXTURE2D_DESC* description, const D3D11_SUBRESOURCE_DATA* initialData, ID3D11Texture2D** texture);
	bool CreateTexture3D(D3D11_TEXTURE3D_DESC* description, const D3D11_SUBRESOURCE_DATA* initialData, ID3D11Texture3D** texture);

	bool CreateRenderTargetView(ID3D11Resource* renderTargetTexture, const D3D11_RENDER_TARGET_VIEW_DESC* renderTargetViewDesc, ID3D11RenderTargetView** renderTargetView);
	bool CreateShaderResourceView(ID3D11Resource* shaderResourceTexture, const D3D11_SHADER_RESOURCE_VIEW_DESC* shaderResourceViewDesc, ID3D11ShaderResourceView** shaderResourceView);
	bool CreateDepthStencilView(ID3D11Resource* depthStencilTexture, const D3D11_DEPTH_STENCIL_VIEW_DESC* description, ID3D11DepthStencilView** depthStencilView);

	// -----------------------------------------

	// Binding render targets for offscreen rendering functionality
	void SetRenderTargets(unsigned int numberToBind, ID3D11RenderTargetView* const* renderTargetViewsToBind, ID3D11DepthStencilView* depthStencilViewToBind);
	void SetDefaultRenderTarget();
	void SetPriorRenderTarget();

	void SetNoDepthStencilBuffer();

	void DisableColourBufferRendering();
	void EnableColourBufferRendering();

	void DisableDepthBufferingRendering();
	void EnableDepthBufferingRendering();

	void DisableStencilBufferingRendering();
	void EnableStencilBufferingRendering();

	void DisableDepthStencilBufferRendering();
	void EnableDepthStencilBufferRendering();

	void ClearRenderTargetView(ID3D11RenderTargetView* renderTargetToClear, const float colour[4]);
	void ClearDepthStencilView(ID3D11DepthStencilView* depthStencilViewToClear, unsigned int clearFlags, float depth, UINT8 stencil);

	// -----------------------------------------

	// Draw calls
	bool Draw(unsigned int vertexCount, unsigned int startLocation);
	bool DrawIndexed(unsigned int numberOfIndicies, unsigned int startIndexLocation, int baseVertexLocation);
	bool DrawInstanced(unsigned int vertexCountPerInstance, unsigned int numberOfInstancesToDraw, unsigned int startVertexLocation, unsigned int startInstanceLocation);

	// -----------------------------------------

	void SetViewport(float width, float height, float minDepth, float maxDepth, unsigned int topLeftX = 0, unsigned int topLeftY = 0);

	void BindRasterizerState(ID3D11RasterizerState* state);
	void CreateRasterizerState(ID3D11RasterizerState** state, 
		                       D3D11_FILL_MODE         fillMode = D3D11_FILL_SOLID, 
		                       D3D11_CULL_MODE		   cullMode = D3D11_CULL_BACK, 
		                       bool                    windingCounterClockwise = false, 
		                       int                     depthBias = 0, 
		                       float                   depthBiasClamp = 0.0f, 
		                       float                   slopeScaledDepthBias = 0.0f, 
		                       bool                    depthClipEnabled = true, 
		                       bool                    scissorEnabled = false, 
		                       bool                    multiSampleEnabled = false, 
		                       bool                    antialiasedLineEnabled = false);

	// Compute Shaders
	void DispatchComputeShader(unsigned int threadGroupCountX, unsigned int threadGroupCountY, unsigned int threadGroupCountZ);

	void BindComputeShader(ID3D11ComputeShader* shader);
	void BindShaderResouceToComputeShader(unsigned int startSlot, unsigned int numberOfViews, ID3D11ShaderResourceView* const* views);

	bool CreateUnorderedAccessView(ID3D11Resource* resource, D3D11_UNORDERED_ACCESS_VIEW_DESC* desc, ID3D11UnorderedAccessView** viewReturn);
	void BindUnorderedAccessViewToComputeShader(unsigned int startSlot, unsigned int numberToBind, ID3D11UnorderedAccessView* const* accessViews, const unsigned int* UAVInitialCounts);
	void UnbindUnorderedAccessViewFromComputeShader(unsigned int startSlot, unsigned int count);

	bool CreateComputeShader(LPCWSTR fileName, LPCSTR entryPoint, ID3D11ComputeShader** computeShaderReturn);

	// Alpha blend state code
	void CreateBlendState(const D3D11_BLEND_DESC* desc, ID3D11BlendState** returnBlendState);
	void CreateBlendState(ID3D11BlendState** returnBlendState, 
                          bool               blendEnabled		   = true,  
                          D3D11_BLEND        srcBlend			   = D3D11_BLEND_SRC1_COLOR,  
                          D3D11_BLEND        destBlend			   = D3D11_BLEND_BLEND_FACTOR, 
		                  D3D11_BLEND_OP     blendOp			   = D3D11_BLEND_OP_ADD,
                          D3D11_BLEND        srcBlendAlpha		   = D3D11_BLEND_ONE,
                          D3D11_BLEND        destBlendAlpha		   = D3D11_BLEND_ZERO,
                          D3D11_BLEND_OP     blendOpAlpha		   = D3D11_BLEND_OP_ADD,
                          UINT8              renderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL);
	 
	void BindBlendState(ID3D11BlendState* blendState, float blendFactor[4], UINT sampleMask = 0xffffffff);

	void BindDefaultBlendState();
	void BindPriorBlendState();

private:
	// Shader compilation 
	bool CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);


	ID3D11Device*        mDeviceHandle;   // Device handle - used for creating the input layout for shaders
	ID3D11DeviceContext* mDeviceContext;  // The device context - used for setting the input layout for the shaders

	// Default
	ID3D11RenderTargetView* mDefaultBackBuffer;
	ID3D11DepthStencilView* mDefaultDepthStencilBuffer;

	ID3D11BlendState*       mDefaultBlendState;

	// Current
	ID3D11RenderTargetView* mCurrentRenderBuffer;
	ID3D11DepthStencilView* mCurrentDepthStencilBuffer;

	ID3D11BlendState*       mCurrentBlendState;
	float                   mCurrentBlendFactor[4];

	// Prior
	ID3D11RenderTargetView* mPriorRenderBuffer;
	ID3D11DepthStencilView* mPriorDepthStencilBuffer;

	ID3D11BlendState*       mPriorBlendState;
	float                   mPriorBlendFactor[4];

};

// ----------------------------------------------------------------------------------------------- /

#endif