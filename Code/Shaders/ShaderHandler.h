#ifndef _SHADER_HANDLER_H_
#define _SHADER_HANDLER_H_

#include <windows.h>

#include <d3d11_1.h>
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
	ShaderHandler(ID3D11Device* deviceHandle, ID3D11DeviceContext* deviceContextHandle);
	~ShaderHandler();

	// Create the actual shaders of each type
	VertexShaderReturnData CompileVertexShader(WCHAR* filePathToOverallShader, LPCSTR nameOfVertexMainFunction);
	ID3D11PixelShader*     CompilePixelShader(WCHAR* filePathToOverallShader, LPCSTR nameOfPixelMainFunction);

	// Setting how the device will be accessing from shader buffers
	bool SetDeviceInputLayout(ID3DBlob* vertexShaderBlob);

	// Buffer creation
	bool CreateBuffer(D3D11_USAGE usageType, D3D11_BIND_FLAG bindFlags, D3D11_CPU_ACCESS_FLAG  CPUAccessFlag, void* bufferData, unsigned int bytesInBuffer, ID3D11Buffer** returnBuffer);

	// Buffer binding functionality
	bool BindVertexBuffersToRegisters(unsigned int startSlot, unsigned int numberOfBuffers, ID3D11Buffer* const* buffers, const unsigned int* strides, const unsigned int* offsets);
	bool BindIndexBuffersToRegisters(ID3D11Buffer* indexBuffer, DXGI_FORMAT format, unsigned int offset);

	bool UpdateSubresource(ID3D11Resource* destResource, unsigned int destSubResource, const D3D11_BOX* destBox, const void* sourceData, unsigned int sourceRowPitch, unsigned int sourceDepthPitch);

	// Data topology type setter
	bool SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);

	// Current Shader Setter
	bool SetVertexShader(ID3D11VertexShader* vertexShader);
	bool SetPixelShader(ID3D11PixelShader* pixelShader);

	// Constant buffer setter
	bool SetVertexShaderConstantBufferData(unsigned int startSlot, unsigned int numberOfbuffers, ID3D11Buffer* const* buffers);
	bool SetPixelShaderConstantBufferData(unsigned int startSlot, unsigned int numberOfbuffers, ID3D11Buffer* const* buffers);

	// Draw calls
	bool DrawIndexed(unsigned int numberOfIndicies, unsigned int startIndexLocation, int baseVertexLocation);


private:
	// Shader compilation 
	bool CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);


	ID3D11Device*        mDeviceHandle;   // Device handle - used for creating the input layout for shaders
	ID3D11DeviceContext* mDeviceContext;  // The device context - used for setting the input layout for the shaders
};

// ----------------------------------------------------------------------------------------------- /

#endif