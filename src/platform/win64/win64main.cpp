/**
 * Description:  Windows x64 main
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 12, 2022 
*/
#if WINDOWS
#include "win64main.hpp"
#include "core/app.hpp"
#include "platform/renderer.hpp"
#include "util.hpp"

// IMPORTANT(alicia): TEMPP!!!!!!!
#include <math.h>

HMODULE OPENGL_MODULE;
HDC DEVICE_CONTEXT;
void WinSwapBuffers() {
    SwapBuffers( DEVICE_CONTEXT );
}

i32 APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE, PSTR, i32 ) {
    INIT_CONSOLE();

    i32 windowWidth  = 1280;
    i32 windowHeight = 720;
#if DEBUG
    const char* windowTitle = "Model Viewer 2.0 | DEBUG";
#else
    const char* windowTitle = "Model Viewer 2.0";
#endif

    u64 perfFrequency; {
        LARGE_INTEGER perfFrequencyLI;
        QueryPerformanceFrequency( &perfFrequencyLI );
        perfFrequency = perfFrequencyLI.QuadPart;
    }

    Core::AppContext app = Core::CreateContext();
    HWND window = WinCreateWindow(
        hInstance,
        windowTitle,
        windowWidth,
        windowHeight,
        &app
    );

    if( !window ) {
        return ERROR_RETURN_CODE;
    }

    HDC deviceContext = GetDC( window );
    if( !deviceContext ) {
        LOG_WINDOWS_ERROR();
        return ERROR_RETURN_CODE;
    }

    HGLRC openGLContext;

    // TODO(alicia): check settings.ini for backend to use
    OPENGL_MODULE = LoadLibrary( L"opengl32.dll" );
    if(!LoadWGLFunctions( OPENGL_MODULE )) {
        return ERROR_RETURN_CODE;
    }
    DEVICE_CONTEXT = deviceContext;
    openGLContext  = WinCreateOpenGLContext( DEVICE_CONTEXT );
    if( !Platform::CreateOpenGLAPI( &app.renderer.api, WinOpenGLLoadProc ) ) {
        LOG_ERROR("Windows x64 > Failed to create OpenGL API!");
        return ERROR_RETURN_CODE;
    }
    app.renderer.api.SwapBuffers = WinSwapBuffers;
    app.renderer.api.Initialize();
    FreeModule( OPENGL_MODULE );

    app.isRunning = true;
    u64 startTime = WinGetTime();
    while( app.isRunning ) {
        f32 elapsedTime = (f32)(WinGetTime() - startTime) / (f32)perfFrequency;
        app.time.deltaTime = elapsedTime - app.time.elapsedTime;
        app.time.elapsedTime = elapsedTime;

        WinProcessMessages( window, &app );
        Core::OnUpdate( &app );
    }

    if( openGLContext ) {
        wglMakeCurrent( nullptr, nullptr );
        wglDeleteContext( openGLContext );
    }

    return SUCCESS_RETURN_CODE;
}

void WinProcessMessages( HWND window, Core::AppContext* ) {
    MSG message = {};
    while(PeekMessage(
        &message,
        window,
        0, 0,
        PM_REMOVE
    ) == TRUE) {
        switch( message.message ) {
            default: {
                TranslateMessage( &message );
                DispatchMessage( &message );
            } break;
        }
    }
}

LRESULT WinWindowProc( HWND window, UINT message, WPARAM wParam, LPARAM lParam ) {
    // NOTE(alicia): user data pointer seems to be null on the first call of WindowProc so
    // in that case, just return DefWindowProc

    Core::AppContext* appContext = (Core::AppContext*)GetWindowLongPtr( window, GWLP_USERDATA );
    if(!appContext || !appContext->isRunning) {
        return DefWindowProc( window, message, wParam, lParam );
    }

    LRESULT result = TRUE;
    switch( message ) {
        case WM_CLOSE: {
            Core::OnClose( appContext );
        } break;
        case WM_WINDOWPOSCHANGED: {
            RECT rect = {};
            if( GetClientRect( window, &rect ) == TRUE ) {
                Core::OnResolutionUpdate( appContext, rect.right, rect.bottom );
            }
        } break;
        default: {
            result = DefWindowProc( window, message, wParam, lParam );
        } break;
    }
    return result;
}

HWND WinCreateWindow(
    HINSTANCE hInstance,
    const char* windowTitle,
    i32 width, i32 height,
    Core::AppContext* appContext
) {
    WNDCLASSEX windowClass   = {};
    windowClass.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc   = WinWindowProc;
    windowClass.hInstance     = hInstance;
    windowClass.hCursor       = LoadCursor( nullptr, IDC_ARROW );
    windowClass.lpszClassName = L"ModelViewerWindowClass";
    // TODO(alicia): set window icon
    windowClass.hIcon         = NULL;
    windowClass.cbClsExtra    = sizeof(appContext);
    windowClass.cbSize        = sizeof(windowClass);

    if( RegisterClassEx( &windowClass ) == FALSE ) {
        LOG_WINDOWS_ERROR();
        return nullptr;
    }

    DWORD dwExStyle = WS_EX_OVERLAPPEDWINDOW;
    DWORD dwStyle   = WS_OVERLAPPEDWINDOW;

    RECT winRect   = {};
    winRect.right  = width;
    winRect.bottom = height;

    if( AdjustWindowRectEx( &winRect, dwStyle, FALSE, dwExStyle ) == FALSE ) {
        LOG_WINDOWS_ERROR();
        return nullptr;
    }

    #define TITLE_BUFFER_SIZE 512
    wchar_t titleBuffer[TITLE_BUFFER_SIZE];
    strTowstr( windowTitle, titleBuffer, TITLE_BUFFER_SIZE );

    HWND result = CreateWindowEx(
        dwExStyle,
        windowClass.lpszClassName,
        titleBuffer,
        dwStyle,
        CW_USEDEFAULT, CW_USEDEFAULT,
        winRect.right  - winRect.left,
        winRect.bottom - winRect.top,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if( !result ) {
        LOG_WINDOWS_ERROR();
        return nullptr;
    }

    SetWindowLongPtr( result, GWLP_USERDATA, (LONG_PTR)appContext );

    ShowWindow( result, SW_SHOWDEFAULT );

    return result;
}

void WinFormatError( char* buffer, usize bufferSize, LPDWORD errorCode ) {
    *errorCode = GetLastError();
    FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        *errorCode,
        0,
        buffer,
        (DWORD)bufferSize,
        nullptr
    );
}

bool LoadWGLFunctions( HMODULE openglModule ) {
    wglCreateContext = (wglFN::wglCreateContextFN)GetProcAddress( openglModule, "wglCreateContext" );
    if( !wglCreateContext ) {
        LOG_ERROR("Windows x64 > Failed to load function \"wglCreateContext\"!");
        return false;
    }
    wglMakeCurrent = (wglFN::wglMakeCurrentFN)GetProcAddress( openglModule, "wglMakeCurrent" );
    if( !wglMakeCurrent ) {
        LOG_ERROR("Windows x64 > Failed to load function \"wglMakeCurrent\"!");
        return false;
    }
    wglDeleteContext = (wglFN::wglDeleteContextFN)GetProcAddress( openglModule, "wglDeleteContext" );
    if( !wglDeleteContext ) {
        LOG_ERROR("Windows x64 > Failed to load function \"wglDeleteContext\"!");
        return false;
    }
    wglGetProcAddress = (wglFN::wglGetProcAddressFN)GetProcAddress( openglModule, "wglGetProcAddress" );
    if( !wglGetProcAddress ) {
        LOG_ERROR("Windows x64 > Failed to load function \"wglGetProcAddress\"!");
        return false;
    }

    return true;
}

HGLRC WinCreateOpenGLContext( HDC deviceContext ) {

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
        LOG_WINDOWS_ERROR();
        return nullptr;
    }

    if( wglMakeCurrent( deviceContext, temp ) == FALSE ) {
        LOG_ERROR("Windows x64 > Failed to make temp OpenGL context current!");
        return nullptr;
    }

    wglCreateContextAttribsARB = (wglFN::wglCreateContextAttribsARBFN)wglGetProcAddress("wglCreateContextAttribsARB");

    if(!wglCreateContextAttribsARB) {
        LOG_ERROR("Windows x64 > Failed to load function \"wglCreateContextAttribsARB\"!");
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

void* WinOpenGLLoadProc( const char* functionName ) {
    void* fn = (void*)wglGetProcAddress( functionName );
    if(!fn) {
        fn = (void*)GetProcAddress( OPENGL_MODULE, functionName );
#if DEBUG
        if(!fn) {
            LOG_WARN( "Windows x64 | OpenGL LoadProc > Failed to load function \"%s\"!", functionName );
        }
#endif
    }
    return fn;
}

HANDLE HEAP_HANDLE;
void* WinAlloc( usize size ) {
    // TODO(alicia): maybe don't use a global here? don't really know how else I would do this tbh
    if(!HEAP_HANDLE) {
        HEAP_HANDLE = GetProcessHeap();
    }

    DEBUG_ASSERT_LOG( HEAP_HANDLE, "Handle to the heap is null!" );

    void* result = HeapAlloc( HEAP_HANDLE, HEAP_ZERO_MEMORY, size );
    DEBUG_ASSERT_LOG( result, "Heap Alloc failed here!" );

    return result;
}

void WinFree( void* mem ) {
    DEBUG_ASSERT_LOG( HEAP_HANDLE, "Handle to the heap is null!" );
    WINBOOL result = HeapFree( HEAP_HANDLE, 0, mem );
    DEBUG_ASSERT_LOG( result != 0, "Heap Free failed here!" );
}

u64 WinGetTime() {
    LARGE_INTEGER counter;
    QueryPerformanceCounter( &counter );
    return counter.QuadPart;
}

#endif