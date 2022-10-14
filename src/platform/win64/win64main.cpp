#ifdef WINDOWS

// ignore compiler warning
// casting function pointers from GetProcAddress/wglGetProcAddress is the intended usage
#pragma GCC diagnostic ignored "-Wcast-function-type"
#include <windows.h>

#include "alias.hpp"
#include "debug.hpp"
#include "global.hpp"
#include "consts.hpp"

#include "core/app.hpp"
#include "platform/event.hpp"
#include "platform/renderer.hpp"

using namespace Platform;

// TODO: remove from win64 layer, should be platform-independent
const wchar_t* WINDOW_NAME = L"Model Viewer";

void Win64InitConsole();
HWND Win64CreateWindow( HINSTANCE hInst, const wchar_t* windowName, i32 width, i32 height );

HGLRC Win64CreateOpenGLContext( HDC deviceContext );
void  Win64DeleteOpenGLContext( HGLRC openglContext );
HINSTANCE OPENGL_MODULE;
void* Win64LoadOpenGLFunctions( const char* functionName );
void Win64OpenGLSwapBuffer();

HWND  WINDOW_HANDLE;
HDC   DEVICE_CONTEXT;
HGLRC OPENGL_CONTEXT;

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE, PSTR, int) {
    Core::AppData appData = {};

    // setup
    Win64InitConsole();
    WINDOW_HANDLE = Win64CreateWindow(
        hInst,
        WINDOW_NAME,
        (i32)DEFAULT_WINDOW_WIDTH,
        (i32)DEFAULT_WINDOW_HEIGHT
    );
    if(!WINDOW_HANDLE) {
        LOG_ERROR("Window x64 > Failed to create window!");
        return -1;
    }

    DEVICE_CONTEXT = GetDC( WINDOW_HANDLE );
    if(!DEVICE_CONTEXT) {
        LOG_ERROR("Window x64 > Failed to get hardware device context!");
        return -1;
    }

    PROGRAM_RUNNING = true;

    switch( CURRENT_BACKEND ) {
        case Renderer::Backend::OPENGL: {
            OPENGL_CONTEXT = Win64CreateOpenGLContext( DEVICE_CONTEXT );
            if( !OPENGL_CONTEXT ) {
                return -1;
            }
        } break;
    }

    appData.renderer = Renderer::New( Renderer::Backend::OPENGL );
    // TODO: Parameterize backend
    if( !appData.renderer ) {
        return -1;
    }

    switch( CURRENT_BACKEND ) {
        case Renderer::Backend::OPENGL: {
            appData.renderer->OpenGLSwapBufferFn = Win64OpenGLSwapBuffer;

            OPENGL_MODULE = LoadLibrary( L"opengl32.dll" );
            if( OPENGL_MODULE ) {
                bool loadResult = appData.renderer->LoadOpenGLFunctions( Win64LoadOpenGLFunctions );
                FreeLibrary( OPENGL_MODULE );
                if(!loadResult) {
                    LOG_ERROR("Windows x64 > Failed to load OpenGL functions!");
                    return -1;
                }
            } else {
                LOG_ERROR("Windows x64 > Failed to load OpenGL module!");
                return -1;
            }
        } break;
    }

    // run app
    Core::RunApp(appData);

    // cleanup
    if( OPENGL_CONTEXT ) {
        Win64DeleteOpenGLContext( OPENGL_CONTEXT );
    }
    ReleaseDC( WINDOW_HANDLE, DEVICE_CONTEXT );
    return 0;
}

void Platform::ProcessWindowEvents() {
    MSG message = {};
    if(PeekMessage(
        &message,
        WINDOW_HANDLE,
        0, 0,
        PM_REMOVE
    ) == FALSE) { return; }

    switch( message.message ) {
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
        case WM_WINDOWPOSCHANGED: {
            // TODO: Event system

            // RECT rect = {};
            // if( GetClientRect( windowHandle, &rect ) == TRUE ) {
            //     if( (f32)rect.right != g_WINDOW_SIZE_CHANGE.w ||
            //         (f32)rect.bottom != g_WINDOW_SIZE_CHANGE.h
            //     ) {
            //         g_WINDOW_SIZE_CHANGE.handled = false;
            //         g_WINDOW_SIZE_CHANGE.w = (f32)rect.right;
            //         g_WINDOW_SIZE_CHANGE.h = (f32)rect.bottom;
            //     }
            // }
        } return TRUE;
        default:
            return DefWindowProc( windowHandle, message, wParam, lParam );
    }
}

HWND Win64CreateWindow( HINSTANCE hInst, const wchar_t* windowName, i32 width, i32 height ) {
    WNDCLASSEX windowClass = {};
    windowClass.cbSize        = sizeof(windowClass);
    windowClass.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc   = Win64WindowProc;
    windowClass.hInstance     = hInst;
    // TODO: set window icon
    windowClass.hIcon         = NULL;
    windowClass.hCursor       = LoadCursor( nullptr, IDC_ARROW );
    windowClass.lpszClassName = L"ModelViewerWindowClass";

    if(RegisterClassEx( &windowClass ) == FALSE) {
        LOG_ERROR("Windows x64 > Failed to register window class!");
        return nullptr;
    }

    DWORD dwExStyle = WS_EX_OVERLAPPEDWINDOW;
    DWORD dwStyle   = WS_OVERLAPPEDWINDOW;

    RECT windowRect   = {};
    windowRect.right  = width;
    windowRect.bottom = height;

    if( AdjustWindowRectEx( &windowRect, dwStyle, FALSE, dwExStyle ) == FALSE ) {
        LOG_ERROR("Windows x64 > Failed to adjust window rect!");
        return nullptr;
    }

    HWND result = CreateWindowEx(
        dwExStyle,
        windowClass.lpszClassName,
        windowName,
        dwStyle,
        CW_USEDEFAULT, CW_USEDEFAULT,
        windowRect.right  - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        hInst,
        nullptr
    );

    ShowWindow( result, SW_SHOWDEFAULT );

    return result;
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
        LOG_ERROR("Windows x64 > Failed to set pixel format!");
        return nullptr;
    }

    HGLRC temp = wglCreateContext( deviceContext );
    if(!temp) {
        LOG_ERROR("Windows x64 > Failed to create temp OpenGL context!");
        return nullptr;
    }

    if( wglMakeCurrent( deviceContext, temp ) == FALSE ) {
        LOG_ERROR("Windows x64 > Failed to make temp OpenGL context current!");
        return nullptr;
    }

    wglCreateContextAttribsARBptr wglCreateContextAttribsARB =
        (wglCreateContextAttribsARBptr)wglGetProcAddress("wglCreateContextAttribsARB");
    if(!wglCreateContextAttribsARB) {
        LOG_ERROR("Windows x64 > Failed to load wglCreateContextAttribsARB!");
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
        LOG_ERROR("Windows x64 > wglCreateContextAttribsARB failed to create OpenGL context!");
        return nullptr;
    }
    wglMakeCurrent( deviceContext, result );

    return result;

}

void Win64DeleteOpenGLContext( HGLRC openglContext ) {
    wglDeleteContext( openglContext );
    wglMakeCurrent( nullptr, nullptr );
}

void* Win64LoadOpenGLFunctions( const char* functionName ) {
    PROC glfnptr = wglGetProcAddress( functionName );
    if(!glfnptr) {
        if(OPENGL_MODULE) {
            glfnptr = GetProcAddress( OPENGL_MODULE, functionName );
        }
    }
    return (void*)glfnptr;
}

void Win64OpenGLSwapBuffer() {
    SwapBuffers( DEVICE_CONTEXT );
}

HANDLE g_hConsole;
void Win64InitConsole() {
#ifdef DEBUG

    g_hConsole   = GetStdHandle( STD_OUTPUT_HANDLE );
    DWORD dwMode = 0;
    GetConsoleMode( g_hConsole, &dwMode );
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode( g_hConsole, dwMode );

#endif
}

#endif