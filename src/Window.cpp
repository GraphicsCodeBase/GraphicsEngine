#include "Window.h"

Window::Window(HINSTANCE hInstance, int width, int height, const std::wstring& title)
    : m_hInstance(hInstance)
{
    WNDCLASSW wc = {}; // Use wide version
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = m_className.c_str();

    RegisterClassW(&wc); // Register wide class

    m_hwnd = CreateWindowExW( // Create wide window
        0,
        m_className.c_str(),
        title.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    ShowWindow(m_hwnd, SW_SHOW);
}

Window::~Window()
{
    DestroyWindow(m_hwnd);
    UnregisterClassW(m_className.c_str(), m_hInstance);
}

bool Window::ProcessMessages()
{
    MSG msg = {};
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) // Wide version
    {
        if (msg.message == WM_QUIT)
            return false;

        TranslateMessage(&msg);
        DispatchMessageW(&msg); // Wide version
    }
    return true;
}

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam); // Wide version
}
