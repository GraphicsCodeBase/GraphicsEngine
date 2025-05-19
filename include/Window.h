#pragma once
#pragma once
#include <Windows.h>
#include <string>

class Window
{
public:
    Window(HINSTANCE hInstance, int width, int height, const std::wstring& title);
    ~Window();

    bool ProcessMessages(); // Returns false if quit message is received
    HWND GetHWND() const { return m_hwnd; }

private:
    HWND m_hwnd;
    HINSTANCE m_hInstance;
    std::wstring m_className = L"MyWindowClass";

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
