#ifdef WINDOWS

// ignore compiler warning
// casting function pointers from GetProcAddress/wglGetProcAddress is the intended usage
#pragma GCC diagnostic ignored "-Wcast-function-type"
#include <windows.h>
#include "defines.hpp"
#include "renderer.hpp"
#include "io.hpp"
#include "globals.hpp"
#include "utils.hpp"
#include "core/text.hpp"
#include "core/font.hpp"

// TODO: Define this struct elsewhere so it's generic and not tied to win64
struct WindowDefinition {
    const wchar_t* name;
    i32 width;
    i32 height;
};
HWND Win64CreateWindow(HINSTANCE hInst, const WindowDefinition& definition);
LRESULT Win64WindowProc( HWND, UINT, WPARAM, LPARAM );
void Win64ProcessMessages( HWND );

HGLRC Win64CreateOpenGLContext( HDC );
HINSTANCE glModule;
void* Win64LoadOpenGLFunctions( const char* functionName );

struct WindowSizeChanged {
    bool handled;
    f32 w; f32 h;
};
WindowSizeChanged g_WINDOW_SIZE_CHANGE;

using namespace Platform;

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE, PSTR, int) {
    // initialize windowSizeChange struct
    g_WINDOW_SIZE_CHANGE = {};
    g_WINDOW_SIZE_CHANGE.handled = true;
    // initialize color output
#ifdef DEBUG
    HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
    DWORD dwMode = 0;
    GetConsoleMode( hConsole, &dwMode );
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode( hConsole, dwMode );
#endif

    WindowDefinition windowDefinition = {};
    windowDefinition.name   = L"Model Viewer";
    windowDefinition.width  = 1280;
    windowDefinition.height = 720;

    HWND windowHandle = Win64CreateWindow(hInst, windowDefinition);
    if(!windowHandle) {
        LOG_ERROR("Windows x64 > Failed to create window!");
        return -1;
    }

    ShowWindow(windowHandle, SW_SHOWDEFAULT);
    HDC deviceContext = GetDC( windowHandle );
    if(!deviceContext) {
        LOG_ERROR("Windows x64 > Failed to get hardware device context!");
        return -1;
    }

    IO::File settingsINI = IO::LoadFile("./resources/settings.ini");
    if( settingsINI.contents ) {
        IO::FreeFile(settingsINI);
    }

    HGLRC hglrc;
    switch( g_CURRENT_BACKEND ) {
        case Renderer::API::OPENGL: {
            LOG_INFO("Current Backend: OpenGL Core 4.6");

            hglrc = Win64CreateOpenGLContext( deviceContext );
            if(!hglrc) {
                LOG_ERROR("Windows x64 > Failed to create OpenGL context!");
                return -1;
            }

            glModule = LoadLibrary( L"opengl32.dll" );
            if(glModule) {
                bool loadResult = Renderer::LoadOpenGLFunctions( Win64LoadOpenGLFunctions );
                FreeLibrary( glModule );
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

    Renderer::API* renderer = Renderer::API::Create( g_CURRENT_BACKEND );
    if(!renderer->Initialize()) {
        return -1;
    }

    renderer->SetViewport( glm::vec2( (f32)windowDefinition.width, (f32)windowDefinition.height ) );
    renderer->SetClearColor( glm::vec4(0.1f) );

    Core::Font::Atlas openSansAtlas = {}; {
        if(!Core::Font::LoadFontAtlas(
            "./resources/open_sans/OpenSans-Regular.ttf",
            64.0f, ' ', '~',
            512, openSansAtlas
        )) {
            LOG_ERROR("Failed to load OpenSans Atlas!");
            return -1;
        }
    }

    LOG_INFO( "Font \"%s\" loaded!", openSansAtlas.fontName.c_str() );

    // renderer->UseFont( openSans );

    g_PROGRAM_RUNNING = true;
    while(g_PROGRAM_RUNNING) {
        Win64ProcessMessages( windowHandle );
        if( !g_WINDOW_SIZE_CHANGE.handled ) {
            glm::vec2 newSize( g_WINDOW_SIZE_CHANGE.w, g_WINDOW_SIZE_CHANGE.h );
            renderer->SetViewport( newSize );
            LOG_INFO("Windows x64 > Window size changed to (%.0f, %.0f)", newSize.x, newSize.y);
            g_WINDOW_SIZE_CHANGE.handled = true;
        }

        // render
        renderer->ClearScreen();

        renderer->SetTextScale(0.8f);
        renderer->SetTextAnchor( Core::Text::AnchorHorizontal::CENTER, Core::Text::AnchorVertical::CENTER );

        renderer->SetTextPosition( glm::vec2( 0.5f, 0.5f ) );
        renderer->SetTextColor( glm::vec3( 1.0f, 0.1f, 0.2f ) );
        renderer->RenderText( "the quick brown fox jumps over the lazy dog" );

        renderer->SetTextPosition( glm::vec2( 0.5f, 0.4f ) );
        renderer->SetTextColor( glm::vec4( 0.1f, 1.0f, 0.2f, 0.8f ) );
        renderer->RenderText( "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG" );

        renderer->SetTextPosition( glm::vec2( 0.5f, 0.3f ) );
        renderer->SetTextColor( glm::vec4( 0.1f, 0.2f, 1.0f, 0.6f ) );
        renderer->RenderText( "0123456789~!@#$%^&*()_-+=[]{}|\\.,/;':\"" );

        if( g_CURRENT_BACKEND == Renderer::API::OPENGL ) {
            SwapBuffers(deviceContext);
        }
    }

    delete( renderer );
    switch(g_CURRENT_BACKEND) {
        case Renderer::API::OPENGL: {
            wglDeleteContext( hglrc );
            wglMakeCurrent( nullptr, nullptr );
        } break;
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
        LOG_ERROR("Windows x64 > Failed to register window class!");
        return nullptr;
    }

    DWORD dwExStyle = WS_EX_OVERLAPPEDWINDOW;
    DWORD dwStyle   = WS_OVERLAPPEDWINDOW;

    RECT windowRect   = {};
    windowRect.right  = definition.width;
    windowRect.bottom = definition.height;

    if( AdjustWindowRectEx( &windowRect, dwStyle, FALSE, dwExStyle ) == FALSE ) {
        LOG_ERROR("Windows x64 > Failed to adjust window rect!");
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
            g_PROGRAM_RUNNING = false;
        } return TRUE;
        case WM_WINDOWPOSCHANGED: {
            RECT rect = {};
            if( GetClientRect( windowHandle, &rect ) == TRUE ) {
                if( (f32)rect.right != g_WINDOW_SIZE_CHANGE.w ||
                    (f32)rect.bottom != g_WINDOW_SIZE_CHANGE.h
                ) {
                    g_WINDOW_SIZE_CHANGE.handled = false;
                    g_WINDOW_SIZE_CHANGE.w = (f32)rect.right;
                    g_WINDOW_SIZE_CHANGE.h = (f32)rect.bottom;
                }
            }
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

void* Win64LoadOpenGLFunctions( const char* functionName ) {
    PROC glfnptr = wglGetProcAddress( functionName );
    if(!glfnptr) {
        if(glModule) {
            glfnptr = GetProcAddress( glModule, functionName );
        }
    }
    return (void*)glfnptr;
}

IO::File IO::LoadFile( const char* filePath ) {
    File result     = {};
    result.size     = 0;
    result.contents = nullptr;

    std::wstring wfilePath = Utils::CStringToWString( filePath );
    if( wfilePath.empty() ) {
        return result;
    }

    HANDLE fileHandle = CreateFile(
        wfilePath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0, 0
    );

    if(fileHandle != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER fileSize;
        if( GetFileSizeEx( fileHandle, &fileSize ) == TRUE ) {
            result.size     = (usize)fileSize.QuadPart;
            result.contents = malloc( result.size );

            if( result.contents ) {
                DWORD bytesRead;
                bool readResult = ReadFile(
                    fileHandle,
                    result.contents,
                    result.size,
                    &bytesRead,
                    NULL
                ) && bytesRead == result.size;

                if(!readResult) {
                    LOG_ERROR("Windows x64 > Failed to read file \"%s\"!", filePath);
                    FreeFile( result );
                    result.size = 0;
                }
            }
        }
        CloseHandle( fileHandle );
    } else {
        LOG_ERROR("Windows x64 > Failed to create file handle!");
    }

    return result;
}

void IO::FreeFile( File file ) {
    if(file.contents) {
        free( file.contents );
    }
}

#endif