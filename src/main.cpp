#include <Windows.h>
#include <iostream>
#include <string>
#include "Window.h"
#include "Renderer.h"
#include "Input.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
{
    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);

    std::wcout << L"Console ready!\n";
    
    const int width = 1280;
    const int height = 720;
    const std::wstring title = L"DirectX Triangle Demo";

    // Create the window
    Window window(hInstance, width, height, title);

    // Create the renderer, passing the HWND and window size
    Renderer renderer(window.GetHWND(), width, height);

    // Initialize the renderer
    renderer.Initialize();
    // === INIT INPUT ===
    Input input;

    input.Initialize();
    std::wcout << L"Renderer initialized successfully!" << std::endl;
    // Main loop
    while (window.ProcessMessages())
    {   

		// Update input state// 
        // === UPDATE INPUT EACH FRAME ===
        input.Update();
        // Example: Check keys
        if (input.IsKeyDown('W'))
        {
            std::wcout << L"W is being held down" << std::endl;
        }
        // Render a frame
        renderer.RenderFrame(width, height); // You’ll replace nullptr with the actual framebuffer if needed
    }

    // Clean up explicitly if you want, but the destructor will handle it
    renderer.Shutdown();

    return 0;
}
