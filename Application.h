//#pragma once
//
//#include <windows.h>
//#include <d3d11_1.h>
//#include <d3dcompiler.h>
//#include <directxmath.h>
//#include <directxcolors.h>
//#include "resource.h"
//
//using namespace DirectX;
//
//// ------------------------------------------------------------------------------------------ //
//
//// Vertex data struct
//struct SimpleVertex
//{
//    XMFLOAT3 Pos;
//    XMFLOAT4 Color;
//};
//
//// ------------------------------------------------------------------------------------------ //
//
//// The constant buffer struct
//struct ConstantBuffer
//{
//	XMMATRIX mWorld;
//	XMMATRIX mView;
//	XMMATRIX mProjection;
//};
//
//// ------------------------------------------------------------------------------------------ //
//
//// The overall applicaiton class that handles everything
//class Application
//{
//
//public:
//	Application();
//	~Application();
//
//	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);
//
//	void Update();
//	void Draw();
//
//private:
//	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
//	HRESULT InitDevice();
//	void    Cleanup();
//
//	// Shader stuff
//	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
//	HRESULT InitShadersAndInputLayout();
//	HRESULT InitVertexBuffer();
//	HRESULT InitIndexBuffer();
//
//	UINT _WindowHeight;
//	UINT _WindowWidth;
//
//private:
//	HINSTANCE               _hInst;             // Handle to the of directX?
//	HWND                    _hWnd;              // Window instance handle 
//	D3D_DRIVER_TYPE         _driverType;        // The driver type
//	D3D_FEATURE_LEVEL       _featureLevel;      // The feature level of directX
//
//	ID3D11Device*           _pd3dDevice;        // The device
//	ID3D11DeviceContext*    _pImmediateContext; // The context of the window
//	IDXGISwapChain*         _pSwapChain;        // The chain that allows for the back and front buffers to swap
//	ID3D11RenderTargetView* _pRenderTargetView; // The render target
//
//	ID3D11VertexShader*     _pVertexShader;     // Vertex shader instance
//	ID3D11PixelShader*      _pPixelShader;      // Pixel shader instance
//	ID3D11InputLayout*      _pVertexLayout;     // Layout of the vertex shader data
//	ID3D11Buffer*           _pVertexBuffer;     // Vertex buffer data
//	ID3D11Buffer*           _pIndexBuffer;      // Index buffer
//	ID3D11Buffer*           _pConstantBuffer;   // The constant buffer
//
//	// Matricies for world manipulation
//	XMFLOAT4X4              _world_cube1;
//	XMFLOAT4X4              _world_cube2;
//
//	XMFLOAT4X4              _view;
//	XMFLOAT4X4              _projection;
//};
//
//// ------------------------------------------------------------------------------------------ //
