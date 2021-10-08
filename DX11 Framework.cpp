#include "Application.h"

// ---------------------------------------------------------------------------------------------------- //

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    // Set the compiler to not complain at the lack of usage
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Create the application we are going to be running
	Application * theApp = new Application();

    // If the application failed to initialise then we should leave the program now
	if (FAILED(theApp->Initialise(hInstance, nCmdShow)))
	{
		return -1;
	}

    // Main message loop
    MSG msg = {0};

    // While the window is not being told to close
    while (WM_QUIT != msg.message)
    {
        // Get the next windows message if there is one
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // If there is a message then translate it and act upon it
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // If no windows message then update and draw the game
			theApp->Update();
            theApp->Draw();
        }
    }

    // When the program has been closed we need to clean up the memory we have allocated for the application
	delete theApp;
	theApp = nullptr;

    return (int) msg.wParam;
}

// ---------------------------------------------------------------------------------------------------- //