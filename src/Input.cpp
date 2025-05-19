#include "Input.h"


BYTE Input::s_CurrentKeys[256] = {};
BYTE Input::s_PreviousKeys[256] = {};
//declare static member variables
// Define the static variables
HWND Input::s_WindowHandle = nullptr;
POINT Input::s_MousePosition = { 0, 0 };

void Input::Initialize()
{
    ZeroMemory(s_CurrentKeys, sizeof(s_CurrentKeys));
    ZeroMemory(s_PreviousKeys, sizeof(s_PreviousKeys));
}

void Input::Update()
{
    memcpy(s_PreviousKeys, s_CurrentKeys, sizeof(s_CurrentKeys));

    for (int i = 0; i < 256; ++i)
    {
        // Check if the high bit is set (key is down)
        s_CurrentKeys[i] = (GetAsyncKeyState(i) & 0x8000) ? 1 : 0;
    }

    //to get mouse position.
    memcpy(s_PreviousKeys, s_CurrentKeys, 256);
    if (!GetKeyboardState(s_CurrentKeys))
    {
        // Handle error if needed (optional)
        ZeroMemory(s_CurrentKeys, sizeof(s_CurrentKeys)); // fallback: clear all key states
    }

    if (s_WindowHandle)
    {
        POINT pos;
        if (GetCursorPos(&pos))
        {
            ScreenToClient(s_WindowHandle, &pos); // Convert to client space
            s_MousePosition = pos;
        }
    }
}

bool Input::IsKeyDown(int key)
{
    return s_CurrentKeys[key];
}

bool Input::WasKeyPressed(int key)
{
    return s_CurrentKeys[key] && !s_PreviousKeys[key];
}

bool Input::WasKeyReleased(int key)
{
    return !s_CurrentKeys[key] && s_PreviousKeys[key];
}

void Input::SetWindowHandle(HWND hwnd)
{
	s_WindowHandle = hwnd;
}

DirectX::XMFLOAT2 Input::GetMousePosition()
{
    //use input handle from member variable.

    POINT pt;
	//this will convert the cursor position to client space. using a vec2.
    if (GetCursorPos(&pt))
    {
        ScreenToClient(s_WindowHandle, &pt);
        return DirectX::XMFLOAT2(static_cast<float>(pt.x), static_cast<float>(pt.y));
    }

    return DirectX::XMFLOAT2(0.0f, 0.0f); // fallback if GetCursorPos fails
}

