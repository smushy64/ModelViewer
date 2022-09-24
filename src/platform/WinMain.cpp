#ifdef WINDOWS

#include <windows.h>
#include "defines.hpp"


struct WindowDefinition {
    LPCWSTR name;
    i32 width;
    i32 height;
};
HWND Win64CreateWindow(HINSTANCE hInst, const WindowDefinition& definition);
LRESULT Win64WindowProc( HWND, UINT, WPARAM, LPARAM );
void Win64ProcessMessages( HWND );

bool PROGRAM_RUNNING = true;

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE, PSTR, int) {

    WindowDefinition windowDefinition = {};
    windowDefinition.name   = L"Model Viewer";
    windowDefinition.width  = 1280;
    windowDefinition.height = 720;

    HWND windowHandle = Win64CreateWindow(hInst, windowDefinition);
    if(!windowHandle) {
        // TODO: Logging!
        return -1;
    }

    HDC deviceContext = GetDC( windowHandle );
    if(!deviceContext) {
        // TODO: Logging!
        return -1;
    }

    ShowWindow(windowHandle, SW_SHOWDEFAULT);

    while(PROGRAM_RUNNING) {
        Win64ProcessMessages( windowHandle );
        // TODO: run program
    }

    return 0;
}

HWND Win64CreateWindow(HINSTANCE hInst, const WindowDefinition& definition) {
    WNDCLASSEX windowClass    = {};
    windowClass.cbSize        = sizeof(windowClass);
    windowClass.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc   = Win64WindowProc;
    windowClass.hInstance     = hInst;
    // NOTE: set window icon
    windowClass.hIcon         = NULL;
    windowClass.hCursor       = LoadCursor( nullptr, IDC_ARROW );
    windowClass.lpszClassName = L"ModelViewerWindowClass";

    if(RegisterClassEx( &windowClass ) == FALSE) {
        // TODO: Logging!
        return nullptr;
    }

    DWORD dwExStyle = WS_EX_OVERLAPPEDWINDOW;
    DWORD dwStyle   = WS_OVERLAPPEDWINDOW;

    RECT windowRect   = {};
    windowRect.right  = definition.width;
    windowRect.bottom = definition.height;

    if( AdjustWindowRectEx( &windowRect, dwStyle, FALSE, dwExStyle ) == FALSE ) {
        // TODO: Logging!
        return nullptr;
    }

    HWND result = CreateWindowEx(
        dwExStyle,
        windowClass.lpszClassName,
        definition.name,
        dwStyle,
        CW_USEDEFAULT, CW_USEDEFAULT,
        windowRect.right  - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        hInst,
        nullptr
    );

    return result;
}

void Win64ProcessMessages( HWND windowHandle ) {
    MSG message = {};
    if(PeekMessage(
        &message,
        windowHandle,
        0, 0,
        PM_REMOVE
    ) == FALSE) { return; }

    switch(message.message) {
        default: {
            TranslateMessage(&message);
            DispatchMessage(&message);
        } break;
    }
}

LRESULT Win64WindowProc( HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam ) {
    switch( message ) {
        case WM_CLOSE: {
            PROGRAM_RUNNING = false;
        } return TRUE;
        default:
            return DefWindowProc( windowHandle, message, wParam, lParam );
    }
}

#endif