#ifndef _INPUT_HANDLER_H_
#define _INPUT_HANDLER_H_

#include "../Maths/CommonMaths.h"

#include <windows.h>

class InputHandler
{
public:
	InputHandler();
	~InputHandler();

	// Initialise
	bool Init(HINSTANCE instance, HWND windowInstance, int width, int height);

	// Windows callback input handler
	static void HandleWindowsInput(UINT message, LPARAM lParam);

	// Input state getters
	bool     GetIsKeyPressed(unsigned char keyToCheck);

	Vector2D GetMousePosition() { return Vector2D((float)mMouseX, (float)mMouseY); }
	bool     GetIsMouseButtonPressed(unsigned char buttonToCheck);
	char     GetScrollWheelFrameDelta();
	Vector2D GetMouseMovementDelta();

private:
	RAWINPUTDEVICE mKeyboard; 
	RAWINPUTDEVICE mMouse;

	static unsigned char        mKeyboardState[256];

	// 0 = Left, 1 = right, 2 = middle mouse, 3 = side left closest to user, 4 = side left away from user, 5 = scroll wheel amount
	static char                 mMouseButtons[6];
	static int                  mMouseX;
	static int                  mMouseY;
	static Vector2D             mMouseDelta;

	int mScreenWidth;
	int mScreenHeight;
};

#endif