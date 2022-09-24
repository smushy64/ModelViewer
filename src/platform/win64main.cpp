#ifdef WINDOWS

#include <windows.h>
#include "defines.hpp"
// TODO: Define this struct elsewhere so it's generic and not tied to win64
struct WindowDefinition {
    LPCWSTR name;
    i32 width;
    i32 height;
};
HWND Win64CreateWindow(HINSTANCE hInst, const WindowDefinition& definition);
LRESULT Win64WindowProc( HWND, UINT, WPARAM, LPARAM );
void Win64ProcessMessages( HWND );

HGLRC Win64CreateOpenGLContext( HDC );
HINSTANCE glModule;
void* Win64LoadOpenGLFunctions( const char* functionName );

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

    ShowWindow(windowHandle, SW_SHOWDEFAULT);
    HDC deviceContext = GetDC( windowHandle );
    if(!deviceContext) {
        // TODO: Logging!
        return -1;
    }

    // TODO: abstraction for handling other renderer APIs
    HGLRC hglrc = Win64CreateOpenGLContext( deviceContext );
    if(!hglrc) {
        // TODO: Logging!
        return -1;
    }
    glModule = LoadLibrary( L"opengl32.dll" );
    if(!glModule) {
        // TODO: logging!
        return -1;
    }
    // TODO: glad loader
    FreeLibrary( glModule );

    while(PROGRAM_RUNNING) {
        Win64ProcessMessages( windowHandle );
        // TODO: run program
        SwapBuffers(deviceContext);
    }

    ReleaseDC( windowHandle, deviceContext );
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

typedef HGLRC (*wglCreateContextAttribsARBptr) (HDC, HGLRC, const i32* );

const i32 WGL_CONTEXT_MAJOR_VERSION_ARB          = 0x2091;
const i32 WGL_CONTEXT_MINOR_VERSION_ARB          = 0x2092;
const i32 WGL_CONTEXT_FLAGS_ARB                  = 0x2094;
const i32 WGL_CONTEXT_PROFILE_MASK_ARB           = 0x9126;
const i32 WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB = 0x0002;
const i32 WGL_CONTEXT_CORE_PROFILE_BIT_ARB       = 0x00000001;

HGLRC Win64CreateOpenGLContext( HDC deviceContext ) {
    PIXELFORMATDESCRIPTOR desiredPixelFormat = {};
    u16 pixelFormatSize = sizeof( PIXELFORMATDESCRIPTOR );
    desiredPixelFormat.nSize      = pixelFormatSize;
    desiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
    desiredPixelFormat.nVersion   = 1;
    desiredPixelFormat.dwFlags    = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    desiredPixelFormat.cColorBits = 32;
    desiredPixelFormat.cAlphaBits = 8;
    desiredPixelFormat.iLayerType = PFD_MAIN_PLANE;

    i32 pixelFormatIndex = ChoosePixelFormat( deviceContext, &desiredPixelFormat );
    PIXELFORMATDESCRIPTOR suggestedPixelFormat = {};
    DescribePixelFormat(
        deviceContext, pixelFormatIndex,
        pixelFormatSize, &suggestedPixelFormat
    );

    if( SetPixelFormat(deviceContext, pixelFormatIndex, &suggestedPixelFormat) == FALSE ) {
        // TODO: logging!
        return nullptr;
    }

    HGLRC temp = wglCreateContext( deviceContext );
    if(!temp) {
        // TODO: logging!
        return nullptr;
    }

    if( wglMakeCurrent( deviceContext, temp ) == FALSE ) {
        // TODO: logging!
        return nullptr;
    }

    wglCreateContextAttribsARBptr wglCreateContextAttribsARB =
        (wglCreateContextAttribsARBptr)wglGetProcAddress("wglCreateContextAttribsARB");
    if(!wglCreateContextAttribsARB) {
        // TODO: logging!
        return nullptr;
    }

    i32 attribs[] = {
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        0
    };

    HGLRC result = wglCreateContextAttribsARB( deviceContext, nullptr, attribs );
    wglDeleteContext( temp );
    if(!result) {
        // TODO: logging!
        return nullptr;
    }
    wglMakeCurrent( deviceContext, result );

    return result;

}

void* Win64LoadOpenGLFunctions( const char* functionName ) {
    PROC glfnptr = wglGetProcAddress( functionName );
    if(!glfnptr) {
        if(glModule) {
            glfnptr = GetProcAddress( glModule, functionName );
        }
    }
    return (void*)glfnptr;
}

#endif