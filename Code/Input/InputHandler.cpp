#include "InputHandler.h"

// -------------------------------------------------------------------------------- //

InputHandler::InputHandler() 
	: mDirectInput(nullptr)
	, mKeyboard(nullptr)
	, mMouse(nullptr)
	, mMouseState()
	, mScreenWidth(0)
	, mScreenHeight(0)
	, mMouseX(0)
	, mMouseY(0)
	, mKeyboardState()
{

}

// -------------------------------------------------------------------------------- //

InputHandler::~InputHandler()
{
	// Free up the keyboard
	if (mKeyboard)
	{
		mKeyboard->Unacquire();
		mKeyboard->Release();
		mKeyboard = nullptr;
	}

	if (mMouse)
	{
		mMouse->Unacquire();
		mMouse->Release();
		mMouse = nullptr;
	}

	if (mDirectInput)
	{
		mDirectInput->Release();
		mDirectInput = nullptr;
	}
}

// -------------------------------------------------------------------------------- //

bool InputHandler::Init(HINSTANCE instance, HWND windowInstance, int width, int height)
{
	if (!instance || !windowInstance)
		return false;

	HRESULT result;

	mMouseX       = 0;
	mMouseY       = 0;
	mScreenHeight = height;
	mScreenWidth  = width;

	// Create the direct input system
	result = DirectInput8Create(instance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&mDirectInput, NULL);

	// Error checking
	if (FAILED(result))
	{
		return false;
	}

	// Initialse the keyboard input device
	result = mDirectInput->CreateDevice(GUID_SysKeyboard, &mKeyboard, NULL);

	// Error checking
	if (FAILED(result))
	{
		return false;
	}

	// Now set the data format
	result = mKeyboard->SetDataFormat(&c_dfDIKeyboard);

	// Error checking
	if (FAILED(result))
	{
		return false;
	}

	// Set the cooperative level for the keyboard
	result = mKeyboard->SetCooperativeLevel(windowInstance, DISCL_FOREGROUND | DISCL_EXCLUSIVE);

	// Error checking
	if (FAILED(result))
	{
		return false;
	}

	// Now the keyboard has been setup we can aquire it
	result = mKeyboard->Acquire();

	// Error checking
	if (FAILED(result))
	{
		return false;
	}

	// Now setup the mouse
	result = mDirectInput->CreateDevice(GUID_SysMouse, &mMouse, NULL);

	// Error checking
	if (FAILED(result))
	{
		return false;
	}

	result = mMouse->SetDataFormat(&c_dfDIMouse);
	
	// Error checking
	if (FAILED(result))
	{
		return false;
	}

	result = mMouse->SetCooperativeLevel(windowInstance, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	// Error checking
	if (FAILED(result))
	{
		return false;
	}

	result = mMouse->Acquire();

	// Error checking
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

// -------------------------------------------------------------------------------- //

bool InputHandler::Update()
{
	if (!ReadKeyboard())
	{
		return false;
	}

	if (!ReadMouse())
	{
		return false;
	}

	ProcessInput();

	// Return we have handled the input
	return true;
}

// -------------------------------------------------------------------------------- //

bool InputHandler::ReadKeyboard()
{
	HRESULT result;

	// Read the keyboard state
	result = mKeyboard->GetDeviceState(sizeof(mKeyboardState), (LPVOID)&mKeyboardState);

	if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
	{
		// If the keyboard has lost focus or has not been aquired then we need to get it back
		mKeyboard->Acquire();
	}
	else
		return false;

	return true;
}

// -------------------------------------------------------------------------------- //

bool InputHandler::ReadMouse()
{
	HRESULT result;

	// Read the mouse data
	result = mMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mMouseState);

	if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
	{
		mMouse->Acquire();
	}
	else
		return false;

	return true;
}

// -------------------------------------------------------------------------------- //

void InputHandler::ProcessInput()
{
	// The mouse state stored is an offset from the prior frame, so apply the change
	mMouseX += mMouseState.lX;
	mMouseY += mMouseState.lY;

	// Cap the mouse to the screen
	if (mMouseX < 0) 
		mMouseX = 0;
	else if (mMouseX > mScreenWidth)
		mMouseX = mScreenWidth;

	if (mMouseY < 0)
		mMouseY = 0;
	else if (mMouseY > mScreenHeight)
		mMouseY = mScreenHeight;
}

// -------------------------------------------------------------------------------- //

bool InputHandler::GetIsKeyPressed(unsigned char keyToCheck)
{
	if (mKeyboardState[keyToCheck] & 0x80)
	{
		return true;
	}

	return false;
}

// -------------------------------------------------------------------------------- //