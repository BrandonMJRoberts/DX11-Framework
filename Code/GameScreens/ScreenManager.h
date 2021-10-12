#ifndef _GAMESCREEN_MANAGER_H_
#define _GAMESCREEN_MANAGER_H_

#include <d3d11_1.h> // DirectX include

#include "GameScreen.h"

const int ScreenWidth = 1000;
const int ScreenHeight = 1000;

const float clearColour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

class GameScreenManager final
{
public:
	GameScreenManager(HINSTANCE hInstance, int nCmdShow);
	~GameScreenManager();

	void Update(const float deltaTime);
	void Render();

private:

	// The current gamescreen
	GameScreen* mCurrentScreen;

	bool InitWindow(HINSTANCE hInstance, int nCmdShow);
	bool InitDevice();

	void Cleanup();

	unsigned int GetRefreshRate();

	// DirectX required variables //
	HINSTANCE               _hInst;             // Handle to the of directX?
	HWND                    _hWnd;              // Window instance handle 
	D3D_DRIVER_TYPE         _driverType;        // The driver type
	D3D_FEATURE_LEVEL       _featureLevel;      // The feature level of directX

	ID3D11Device*           _pd3dDevice;        // The device
	ID3D11DeviceContext*    _pImmediateContext; // The context of the window
	IDXGISwapChain*         _pSwapChain;        // The chain that allows for the back and front buffers to swap
	ID3D11RenderTargetView* _pRenderTargetView; // The render target
};

#endif