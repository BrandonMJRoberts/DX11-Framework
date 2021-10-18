#ifndef _INPUT_HANDLER_H_
#define _INPUT_HANDLER_H_

#include <dinput.h>
#include "../Maths/CommonMaths.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#define DIRECTINPUT_VERSION 0x0800

class InputHandler
{
public:
	InputHandler();
	~InputHandler();

	bool Init(HINSTANCE instance, HWND windowInstance, int width, int height);
	bool Update();

	Vector2D GetMousePosition() { return Vector2D((float)mMouseX, (float)mMouseY); }
	bool     GetIsKeyPressed(unsigned char keyToCheck);

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

	IDirectInput8*       mDirectInput;
	IDirectInputDevice8* mKeyboard;
	IDirectInputDevice8* mMouse;

	unsigned char        mKeyboardState[256];
	DIMOUSESTATE         mMouseState;

	int mScreenWidth;
	int mScreenHeight;
	int mMouseX;
	int mMouseY;
};

#endif