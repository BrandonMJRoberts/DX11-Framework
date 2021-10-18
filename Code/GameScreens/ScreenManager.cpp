#include "ScreenManager.h"

#include "../../resource.h"
#include "../Shaders/ShaderHandler.h"

#include "GameScreen_MainMenu.h"
#include "GameScreen_Editor.h"
#include "GameScreen_InGame.h"

#include <iostream>

const int GameScreenManager::ScreenWidth  = 640;
const int GameScreenManager::ScreenHeight = 480;

// -------------------------------------------------------------------------- //

GameScreenManager::GameScreenManager(HINSTANCE hInstance, int nCmdShow)
	: mCurrentScreen(nullptr)
    , mShaderHandler(nullptr)
    , mDeviceHandle(nullptr)
    , mDeviceContextHandle(nullptr)
    , mSwapChain(nullptr)
    , mRenderTargetView(nullptr)
    , mDepthStencilBuffer(nullptr)
    , mDepthStencilView(nullptr)
    , mInputHandler(nullptr)
{
	// Actual windows window setup
    if (!InitWindow(hInstance, nCmdShow))
        return;

    // Now setup the Device
    if (!InitDevice())
    {
        Cleanup();
        return;
    }

    // Create the shader handler to be a wrapper around all needed shader functionality - pass this into things instead of the device handle
    mShaderHandler = new ShaderHandler(mDeviceHandle, mDeviceContextHandle);
    mInputHandler  = new InputHandler();

    if (mInputHandler)
    {
        mInputHandler->Init(mInstanceHandle, mWindowHandle, ScreenWidth, ScreenHeight);
    }
    else
    {
        return;
    }

    if(mShaderHandler)
        SwitchToWindow(ScreenTypes::MAIN_MENU, *mShaderHandler);
    else
    {
        std::cout << "Shader handler failed to initialise!" << std::endl;
    }
}

// -------------------------------------------------------------------------- //

GameScreenManager::~GameScreenManager()
{
	delete mCurrentScreen;
	mCurrentScreen = nullptr;

    delete mShaderHandler;
    mShaderHandler = nullptr;

    delete mInputHandler;
    mInputHandler = nullptr;

    Cleanup();
}

// -------------------------------------------------------------------------- //

void GameScreenManager::Render()
{
    // Clear the screen
    mDeviceContextHandle->ClearRenderTargetView(mRenderTargetView, clearColour);

    // Clear the depth and stencil buffers
    mDeviceContextHandle->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // Render the current screen
    if(mCurrentScreen)
        mCurrentScreen->Render();

    // Present our back buffer to our front buffer - vsynced
    mSwapChain->Present(1, 0);
}

// -------------------------------------------------------------------------- //

void GameScreenManager::Update(const float deltaTime)
{
    if (mInputHandler)
        mInputHandler->Update();

    if (mCurrentScreen)
        mCurrentScreen->Update(deltaTime);
}

// -------------------------------------------------------------------------- //

// Windows messaging callback function
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

// -------------------------------------------------------------------------- //

bool GameScreenManager::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Register class
    WNDCLASSEX wcex;
    wcex.cbSize        = sizeof(WNDCLASSEX);                            // Set the size to set what version of the struct we are going to be using
    wcex.style         = CS_HREDRAW | CS_VREDRAW;                       // Set the style - 1st one sets that the window should redraw if the window is moved or re-sized on the width, 2nd one is the same as the first but for the height
    wcex.lpfnWndProc   = WndProc;                                       // The window callback procedure
    wcex.cbClsExtra    = 0;                                             // The number of extra bytes to allocate following the class structure 
    wcex.cbWndExtra    = 0;                                             // Extra bytes to allocate following window instance
    wcex.hInstance     = hInstance;                                     // Handle to instance that contains the window procedure for the class
    wcex.hIcon         = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);   // Handle to icon class
    wcex.hCursor       = LoadCursor(NULL, IDC_ARROW );                  // Handle to the cursor resource
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);                    // Background brush thing
    wcex.lpszMenuName  = nullptr;                                       // Menu name 
    wcex.lpszClassName = L"WindowClass";                                // Window class name
    wcex.hIconSm       = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1); // Icon 

    // Error checking and class setup for the window
    if (!RegisterClassEx(&wcex))
        return false;

    // Create window
    mInstanceHandle = hInstance;
    RECT rc = {0, 0, ScreenWidth, ScreenHeight}; // Set the screen corner positions

    // Apply this positional data
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    // Create the window
    mWindowHandle = CreateWindow(L"WindowClass", L"Track Builder Project", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);

    // Check that the window has been constructed correctly
    if (!mWindowHandle)
		return false;

    // Display the window
    ShowWindow(mWindowHandle, nCmdShow);

    return true;
}

// -------------------------------------------------------------------------- //

bool GameScreenManager::InitDevice()
{
    UINT createDeviceFlags = 0;

    // Check if the program is running in debug mode
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // ----------------------------------------------------------------------------------------- 

    // Create an array of driver types
    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE
    };

    // Get the size of the array above
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    // The feaure levels we want to support
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };

    // Get the size of the array above
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    // ----------------------------------------------------------------------------------------- 

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount                        = 1;
    sd.BufferDesc.Width                   = ScreenWidth;  // Window width
    sd.BufferDesc.Height                  = ScreenHeight; // Window height
    sd.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Denominator = 1;  
    sd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow                       = mWindowHandle;
    sd.SampleDesc.Count                   = 1;
    sd.SampleDesc.Quality                 = 0;
    sd.Windowed                           = TRUE;             // If the window is run in windowed mode or not
    sd.BufferDesc.RefreshRate.Numerator   = GetRefreshRate(); // Set the refresh rate to being whatever the user's monitor is 

    // ----------------------------------------------------------------------------------------- 

    // Setup the driver
    HRESULT hr;
    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        // Get the current driver type
        mDriverType = driverTypes[driverTypeIndex];

        // Attempt to create the device from the driver
        hr          = D3D11CreateDeviceAndSwapChain(nullptr, mDriverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &mSwapChain, &mDeviceHandle, &mFeatureLevel, &mDeviceContextHandle);

        // If we have succeeded then get out of the loop
        if (SUCCEEDED(hr))
            break;
    }

    // If none of the above have succeeded then we cannot create the device correctly
    if (FAILED(hr))
        return false;

    // ----------------------------------------------------------------------------------------- 

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
                     hr          = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return false;

    hr = mDeviceHandle->CreateRenderTargetView(pBackBuffer, nullptr, &mRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        return false;

    if (SetupDepthStencil())
    {
        mDeviceContextHandle->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
    }
    else
    {
        mDeviceContextHandle->OMSetRenderTargets(1, &mRenderTargetView, nullptr);
    }

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width    = (FLOAT)ScreenWidth;
    vp.Height   = (FLOAT)ScreenHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    mDeviceContextHandle->RSSetViewports(1, &vp);

    return true;
}

// -------------------------------------------------------------------------- //

unsigned int GameScreenManager::GetRefreshRate()
{
    // Get the monitor's refresh rate //
    DEVMODE devMode;
    memset(&devMode, 0, sizeof(DEVMODE)); // Fill in the struct with zero's data

    // Default to 60
    unsigned int refreshRateToReturn = 60;

    devMode.dmSize        = sizeof(DEVMODE); // Set the size of the variable to indicate the type of DEVMODE being used
    devMode.dmDriverExtra = 0; // Set that we are not using any device specific information by setting this to being zero

    if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode) != 0)
    {
        refreshRateToReturn = devMode.dmDisplayFrequency;
    }

    return refreshRateToReturn;
}

// -------------------------------------------------------------------------- //

void GameScreenManager::Cleanup()
{
    if (mDeviceContextHandle) mDeviceContextHandle->ClearState();

    if (mRenderTargetView)    mRenderTargetView->Release();
    if (mSwapChain)           mSwapChain->Release();
    if (mDeviceContextHandle) mDeviceContextHandle->Release();
    if (mDeviceHandle)        mDeviceHandle->Release();

    if (mDepthStencilView)     mDepthStencilView->Release();
    if (mDepthStencilBuffer)   mDepthStencilBuffer->Release();
}

// -------------------------------------------------------------------------- //

void GameScreenManager::SwitchToWindow(ScreenTypes screenType, ShaderHandler& shaderHandler)
{
    if (mCurrentScreen)
    {
        delete mCurrentScreen;
        mCurrentScreen = nullptr;
    }

    switch (screenType)
    {
    default:
    case ScreenTypes::MAIN_MENU:
        mCurrentScreen = new GameScreen_MainMenu(shaderHandler, *mInputHandler);
    break;

    case ScreenTypes::EDITOR:
        mCurrentScreen = new GameScreen_Editor(shaderHandler, *mInputHandler);
    break;

    case ScreenTypes::IN_GAME:
        mCurrentScreen = new GameScreen_InGame(shaderHandler, *mInputHandler);
    break;
    }
}

// -------------------------------------------------------------------------- //

bool GameScreenManager::SetupDepthStencil()
{
    // Define the texture
    D3D11_TEXTURE2D_DESC depthStencilDesc;

    depthStencilDesc.Width     = ScreenWidth;
    depthStencilDesc.Height    = ScreenHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count   = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage              = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags     = 0;
    depthStencilDesc.MiscFlags          = 0;

    // Create the texture and the view
    mDeviceHandle->CreateTexture2D(&depthStencilDesc, nullptr, &mDepthStencilBuffer);

    if (mDepthStencilBuffer)
        mDeviceHandle->CreateDepthStencilView(mDepthStencilBuffer, nullptr, &mDepthStencilView);
    else
    {
        return false;
    }

    return true;

}

// -------------------------------------------------------------------------- //