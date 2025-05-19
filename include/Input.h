#pragma once
#include <DirectXMath.h>
#include <Windows.h>
class Input
{
public:
    static void Initialize();
    static void Update();
    static bool IsKeyDown(int key);
    static bool WasKeyPressed(int key);
    static bool WasKeyReleased(int key);
    // In Input.h
    static void SetWindowHandle(HWND hwnd);
    static DirectX::XMFLOAT2 GetMousePosition();

private:
    static BYTE s_CurrentKeys[256];
    static BYTE s_PreviousKeys[256];
    // In Input.h (private section)
    static POINT s_MousePosition;
    static HWND s_WindowHandle;

};
