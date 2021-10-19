#include "InputHandler.h"

#include <iostream>

// -------------------------------------------------------------------------------- //

unsigned char InputHandler::mKeyboardState[256];
int			  InputHandler::mMouseX = 0;
int			  InputHandler::mMouseY = 0;
char          InputHandler::mMouseButtons[6];

void InputHandler::HandleWindowsInput(UINT message, LPARAM lParam)
{
	switch (message)
	{
	default:
		return;

	case WM_INPUT:
		UINT dwSize = 0;

		if(GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER)) == -1)
		{
			return;
		}

		LPBYTE lpb = new BYTE[dwSize];
		if (lpb == NULL)
		{
			return;
		}

		if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
		{
			// An error has occured as the size returned doesnt match 
			delete[] lpb;
			return;
		}


		// Convert the data into the correct format
		RAWINPUT* rawInput = (RAWINPUT*)lpb;

		// If we are dealing with a keyboard
		if (rawInput->header.dwType == RIM_TYPEKEYBOARD)
		{
			UINT keyID = MapVirtualKey(rawInput->data.keyboard.VKey, MAPVK_VK_TO_CHAR);

			// First check to see if the message is a key down
			if (rawInput->data.keyboard.Flags == RI_KEY_MAKE) // Press
			{
				mKeyboardState[keyID] = 1;
			}
			else if (rawInput->data.keyboard.Flags == RI_KEY_BREAK) // Release
			{
				mKeyboardState[keyID] = 0;
			}
		}
		else if (rawInput->header.dwType == RIM_TYPEMOUSE) 
		{
			mMouseX += rawInput->data.mouse.lLastX;
			mMouseY += rawInput->data.mouse.lLastY;

			switch (rawInput->data.mouse.usButtonFlags)
			{
			// Button 1
			case RI_MOUSE_BUTTON_1_DOWN:
				mMouseButtons[0] = 1;
			break;

			case RI_MOUSE_BUTTON_1_UP:
				mMouseButtons[0] = 0;
			break;

			// Button 2
			case RI_MOUSE_BUTTON_2_DOWN:
				mMouseButtons[1] = 1;
				break;

			case RI_MOUSE_BUTTON_2_UP:
				mMouseButtons[1] = 0;
			break;

			// Button 3
			case RI_MOUSE_BUTTON_3_DOWN:
				mMouseButtons[2] = 1;
			break;

			case RI_MOUSE_BUTTON_3_UP:
				mMouseButtons[2] = 0;
			break;

			// Button 4
			case RI_MOUSE_BUTTON_4_DOWN:
				mMouseButtons[3] = 1;
			break;

			case RI_MOUSE_BUTTON_4_UP:
				mMouseButtons[3] = 0;
			break;

			// Button 5
			case RI_MOUSE_BUTTON_5_DOWN:
				mMouseButtons[4] = 1;
			break;

			case RI_MOUSE_BUTTON_5_UP:
				mMouseButtons[4] = 0;
			break;

			// Mouse wheel
			case RI_MOUSE_WHEEL:
				// Apply the delta
				mMouseButtons[5] += rawInput->data.mouse.usButtonData;
			break;
			}
		}

		delete[] lpb;
	return;
	}
}

// -------------------------------------------------------------------------------- //

InputHandler::InputHandler() 
	: mScreenWidth(0)
	, mScreenHeight(0)
	, mKeyboard()
	, mMouse()
{

}

// -------------------------------------------------------------------------------- //

InputHandler::~InputHandler()
{
	
}

// -------------------------------------------------------------------------------- //

bool InputHandler::Init(HINSTANCE instance, HWND windowInstance, int width, int height)
{
	if (!instance || !windowInstance)
		return false;

	mMouseX       = 0;
	mMouseY       = 0;
	mScreenHeight = height;
	mScreenWidth  = width;

	// ------------------------------------------------------------------------------------------

	// Setup the mouse
	mMouse.usUsagePage = 0x01;
	mMouse.usUsage     = 0x02;
	mMouse.dwFlags     = RIDEV_INPUTSINK;
	mMouse.hwndTarget  = windowInstance;
	
	// Now the keyboard
	mKeyboard.usUsagePage = 0x01;
	mKeyboard.usUsage     = 0x06;
	mKeyboard.dwFlags     = RIDEV_NOLEGACY;
	mKeyboard.hwndTarget  = windowInstance;

	if (!RegisterRawInputDevices(&mKeyboard, 1, sizeof(mKeyboard)))
	{
		// Failed to register the keyboard
		return false;
	}

	if (!RegisterRawInputDevices(&mMouse, 1, sizeof(mMouse)))
	{
		// Failed to register the mouse
		return false;
	}

	// ------------------------------------------------------------------------------------------

	return true;
}

// -------------------------------------------------------------------------------- //

bool InputHandler::GetIsKeyPressed(unsigned char keyToCheck)
{
	return mKeyboardState[keyToCheck];
}

// -------------------------------------------------------------------------------- //

bool InputHandler::GetIsMouseButtonPressed(unsigned char buttonToCheck)
{
	if (buttonToCheck > 4)
		return false;

	return mMouseButtons[buttonToCheck];
}

// -------------------------------------------------------------------------------- //

char InputHandler::GetScrollWheelFrameDelta()
{
	char returnVal = mMouseButtons[5];

	// Reset the scroll
	mMouseButtons[5] = 0;

	return returnVal;
}

// -------------------------------------------------------------------------------- //