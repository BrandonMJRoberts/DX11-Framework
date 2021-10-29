#ifndef _GAMESCREEN_MANAGER_H_
#define _GAMESCREEN_MANAGER_H_

#include <d3d11_1.h> // DirectX include

#include "GameScreen.h"
#include "../Input/InputHandler.h"

const float clearColour[4] = { 0.0f, 0.125f, 0.3f, 1.0f };

// ----------------------------------------------------------------- //

enum class ScreenTypes
{
	MAIN_MENU = 0,
	EDITOR,
	IN_GAME
};

// ----------------------------------------------------------------- //

class ShaderHandler;

class GameScreenManager final
{
public:
	GameScreenManager(HINSTANCE hInstance, int nCmdShow);
	~GameScreenManager();

	void Update(const float deltaTime);
	void Render();

	void SwitchToWindow(ScreenTypes screenType, ShaderHandler& shaderHandler);

	static const int ScreenWidth;
	static const int ScreenHeight;

private:

	// The current gamescreen
	GameScreen* mCurrentScreen;

	bool InitWindow(HINSTANCE hInstance, int nCmdShow);
	bool InitDevice();
	bool SetupDepthStencil();
	void Cleanup();

	unsigned int GetRefreshRate();

	ShaderHandler* mShaderHandler;
	InputHandler*  mInputHandler;

	// DirectX required variables //
	HINSTANCE               mInstanceHandle;     // Handle to the of directX?
	HWND                    mWindowHandle;       // Window instance handle 
	D3D_DRIVER_TYPE         mDriverType;         // The driver type
	D3D_FEATURE_LEVEL       mFeatureLevel;       // The feature level of directX

	ID3D11DepthStencilView* mDepthStencilView;   // The depth/stencil view that acts as a depth buffer

	ID3D11Device*           mDeviceHandle;       // The device
	ID3D11DeviceContext*    mDeviceContextHandle;// The context of the window

	IDXGISwapChain*         mSwapChain;          // The chain that allows for the back and front buffers to swap
	ID3D11RenderTargetView* mBackBuffer;         // The render target
};

// ----------------------------------------------------------------- //

#endif