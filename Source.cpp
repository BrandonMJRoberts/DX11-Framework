#include "Code/GameScreens/ScreenManager.h"

#include <windows.h>

// ---------------------------------------------------------------------------------------------------- //

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    // Set the compiler to not complain at the lack of usage
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Create the application we are going to be running
    GameScreenManager* gameScreenManager = new GameScreenManager(hInstance, nCmdShow);

    // Check the screen was setup correctly
    if (gameScreenManager == nullptr)
    {
        return -1;
    }

    // Main message loop
    MSG msg = {0};

    DWORD dwTimeStart = 0;
    DWORD dwTimeCur   = 0;
    float deltaTime   = 0.0f;

    float frameRate;

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
            // Calculate deltaTime
            dwTimeStart = dwTimeCur;
            dwTimeCur  = GetTickCount();

            if (dwTimeStart == 0)
                dwTimeStart = dwTimeCur;

            deltaTime = (dwTimeCur - dwTimeStart) / 1000.0f;

            // If no windows message then update and render the game
            gameScreenManager->Update(deltaTime);
            gameScreenManager->Render();

            frameRate = 1.0f / deltaTime;
        }
    }

    // When the program has been closed we need to clean up the memory we have allocated for the application
	delete gameScreenManager;
    gameScreenManager = nullptr;

    return (int) msg.wParam;
}

// ---------------------------------------------------------------------------------------------------- //