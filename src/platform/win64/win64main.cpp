/**
 * Description:  Windows x64 main
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 12, 2022 
*/
#if WINDOWS
#include <windowsx.h>
#include "win64main.hpp"
#include "core/app.hpp"
#include "platform/renderer.hpp"
#include "util.hpp"
#include "platform/io.hpp"

Platform::CursorStyle CURSOR_STYLE = Platform::CursorStyle::ARROW;
Platform::CursorStyle DESIRED_CURSOR_STYLE = Platform::CursorStyle::ARROW;
bool CURSOR_VISIBLE = true;
bool CURSOR_LOCKED  = false;
bool APP_ACTIVE     = true;

HDC DEVICE_CONTEXT;
void WinSwapBuffers() {
    SwapBuffers( DEVICE_CONTEXT );
}

static i32 WINDOW_WIDTH  = 1280;
static i32 WINDOW_HEIGHT = 720;

void CenterCursor( HWND window ) {
    POINT screen = {};
    screen.x = WINDOW_WIDTH;
    screen.y = WINDOW_HEIGHT;
    ClientToScreen( window, &screen );
    SetCursorPos( screen.x, screen.y );
}

i32 APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE, PSTR, i32 ) {
    INIT_CONSOLE();

#if DEBUG
    const char* windowTitle = "Model Viewer 2.0 | DEBUG";
#else
    const char* windowTitle = "Model Viewer 2.0";
#endif

    // TODO(alicia): parse settings
    Platform::RendererBackend backend = Platform::RendererBackend::OPENGL;

    u64 perfFrequency; {
        LARGE_INTEGER perfFrequencyLI;
        QueryPerformanceFrequency( &perfFrequencyLI );
        perfFrequency = perfFrequencyLI.QuadPart;
    }

    Core::AppContext app = Core::CreateContext();
    HWND window = WinCreateWindow(
        hInstance,
        windowTitle,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
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
    switch( backend ) {
        case Platform::RendererBackend::OPENGL: {
            HMODULE openglModule = LoadLibrary( L"opengl32.dll" );
            if(!WinLoadWGLFunctions( openglModule )) {
                return ERROR_RETURN_CODE;
            }
            DEVICE_CONTEXT = deviceContext;
            openGLContext  = WinCreateOpenGLContext( DEVICE_CONTEXT );
            if( !Platform::CreateOpenGLAPI( &app.rendererAPI, WinOpenGLLoadProc ) ) {
                LOG_ERROR("Windows x64 > Failed to create OpenGL API!");
                return ERROR_RETURN_CODE;
            }
            app.rendererAPI.SwapBuffers = WinSwapBuffers;
            FreeModule( openglModule );
        } break;
        default: {
            LOG_ERROR("Windows x64 > \"%s\" is not yet supported!", Platform::RendererBackendToString( backend ));
            return ERROR_RETURN_CODE;
        } break;
    }

    Core::OnInit( &app );
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

void WinProcessMessages( HWND window, Core::AppContext* appContext ) {
    MSG message = {};
    while(PeekMessage(
        &message,
        window,
        0, 0,
        PM_REMOVE
    ) == TRUE) {
        switch( message.message ) {
            case WM_MOUSEMOVE: {
                if( CURSOR_LOCKED ) {
                    CenterCursor( window );
                    appContext->input.pixelMousePos = { WINDOW_WIDTH/2, WINDOW_HEIGHT/2 };
                    appContext->input.screenMousePos = { 0.5f, 0.5f };
                } else {
                    smath::ivec2 pixelPos = { (i32)GET_X_LPARAM(message.lParam), WINDOW_HEIGHT - (i32)GET_Y_LPARAM(message.lParam) };
                    smath::vec2 screenPos = { (f32)pixelPos.x / (f32)WINDOW_WIDTH, (f32)pixelPos.y /(f32)WINDOW_HEIGHT };
                    appContext->input.pixelMousePos  = pixelPos;
                    appContext->input.screenMousePos = screenPos;
                }
            } break;
            case WM_KEYDOWN:
            case WM_KEYUP: {
                Platform::KeyCode key = VKCodeToKeyCode( (u32)message.wParam );
                bool isDown = ( message.lParam & ( 1 << 31) ) == 0;
                appContext->input.SetKey( key, isDown );
            } break;
            case WM_LBUTTONDOWN: {
                appContext->input.leftMouse = true;
            } break;
            case WM_LBUTTONUP: {
                appContext->input.leftMouse = false;
            } break;
            case WM_RBUTTONDOWN: {
                appContext->input.rightMouse = true;
            } break;
            case WM_RBUTTONUP: {
                appContext->input.rightMouse = false;
            } break;
            case WM_MBUTTONDOWN: {
                appContext->input.middleMouse = true;
            } break;
            case WM_MBUTTONUP: {
                appContext->input.middleMouse = false;
            } break;
            case WM_MOUSEWHEEL: {
                appContext->input.mouseWheel = GET_WHEEL_DELTA_WPARAM( message.wParam ) / WHEEL_DELTA;
            } break;
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
                WINDOW_WIDTH  = rect.right;
                WINDOW_HEIGHT = rect.bottom;
                Core::OnResolutionUpdate( appContext, rect.right, rect.bottom );
            }
        } break;
        case WM_ACTIVATEAPP: {
            APP_ACTIVE = wParam == TRUE;
            if( APP_ACTIVE ) {
                Core::OnAppActivated( appContext );
            } else {
                Core::OnAppDeactivated( appContext );
            }
        } break;
        case WM_SETCURSOR: { switch( LOWORD( lParam ) ) {
            case HTRIGHT:
            case HTLEFT: {
                DESIRED_CURSOR_STYLE = Platform::CursorStyle::RESIZE_HORIZONTAL;
            } break;
            case HTTOP:
            case HTBOTTOM: {
                DESIRED_CURSOR_STYLE = Platform::CursorStyle::RESIZE_VERTICAL;
            } break;
            case HTBOTTOMLEFT:
            case HTTOPRIGHT: {
                DESIRED_CURSOR_STYLE = Platform::CursorStyle::RESIZE_RIGHT;
            } break;
            case HTBOTTOMRIGHT:
            case HTTOPLEFT: {
                DESIRED_CURSOR_STYLE = Platform::CursorStyle::RESIZE_LEFT;
            } break;
            default: {
                DESIRED_CURSOR_STYLE = Platform::CursorStyle::ARROW;
            } break;
        } } return TRUE;
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
    windowClass.hCursor       = LoadCursor( hInstance, IDC_ARROW );
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

bool WinLoadWGLFunctions( HMODULE openglModule ) {
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
        HMODULE openglModule = GetModuleHandle( L"opengl32.dll" );
        DEBUG_ASSERT_LOG( openglModule,
            "Windows x64 | OpenGL LoadProc > OpenGL module was not loaded before loading OpenGL functions!"
        );
        fn = (void*)GetProcAddress( openglModule, functionName );
#if DEBUG
        if(!fn) {
            LOG_WARN( "Windows x64 | OpenGL LoadProc > Failed to load function \"%s\"!", functionName );
        }
#endif
    }
    return fn;
}

HANDLE HEAP_HANDLE;
void* Platform::Alloc( usize size ) {
    // NOTE(alicia): maybe don't use a global here? don't really know how else I would do this tbh
    if(!HEAP_HANDLE) {
        HEAP_HANDLE = GetProcessHeap();
    }

    DEBUG_ASSERT_LOG( HEAP_HANDLE, "Handle to the heap is null!" );

    void* result = HeapAlloc( HEAP_HANDLE, HEAP_ZERO_MEMORY, size );
    DEBUG_ASSERT_LOG( result, "Heap Alloc failed here!" );

    return result;
}

void Platform::Free( void* mem ) {
    DEBUG_ASSERT_LOG( HEAP_HANDLE, "Handle to the heap is null!" );
    WINBOOL result = HeapFree( HEAP_HANDLE, 0, mem );
    DEBUG_ASSERT_LOG( result != 0, "Heap Free failed here!" );
}

void Platform::AppendToWindowTitle( const char* append, usize appendLen ) {
    HWND window = GetActiveWindow();
    if(!window) {
        LOG_ERROR("Windows x64 | AppendToWindowTitle > Failed to get window handle!");
        return;
    }

    wchar_t appendW[appendLen];
    strTowstr( append, appendW, appendLen );

    const usize MAX_WINDOW_TITLE_BUFFER = 512;
    wchar_t windowTitle[MAX_WINDOW_TITLE_BUFFER];
    GetWindowText( window, windowTitle, MAX_WINDOW_TITLE_BUFFER );
    usize windowTitleLen = strLenW( windowTitle );

    usize newTitleLen = appendLen + windowTitleLen;
    wchar_t newTitle[newTitleLen];
    strConcatW(
        windowTitleLen, windowTitle,
        appendLen, appendW,
        newTitleLen, newTitle
    );
    SetWindowText( window, newTitle );
    CloseHandle( window );
}

u64 WinGetTime() {
    LARGE_INTEGER counter;
    QueryPerformanceCounter( &counter );
    return counter.QuadPart;
}

LPCTSTR WinCursorStyle( Platform::CursorStyle style ) {
    switch( style ) {
        case Platform::CursorStyle::HAND:              return IDC_HAND;
        case Platform::CursorStyle::WAIT:              return IDC_WAIT;
        case Platform::CursorStyle::BEAM:              return IDC_IBEAM;
        case Platform::CursorStyle::NOT_ALLOWED:       return IDC_NO;
        case Platform::CursorStyle::RESIZE_RIGHT:      return IDC_SIZENESW;
        case Platform::CursorStyle::RESIZE_LEFT:       return IDC_SIZENWSE;
        case Platform::CursorStyle::RESIZE_VERTICAL:   return IDC_SIZENS;
        case Platform::CursorStyle::RESIZE_HORIZONTAL: return IDC_SIZEWE;
        default: return IDC_ARROW;
    }
}
void Platform::SetCursorStyle( CursorStyle style ) {
    if( CURSOR_STYLE == style ) {
        return;
    }
    SetCursor( LoadCursor( nullptr, WinCursorStyle( style ) ) );
    CURSOR_STYLE = style;
}
void Platform::ResetCursorStyle() {
    SetCursorStyle( DESIRED_CURSOR_STYLE );
}
Platform::CursorStyle Platform::GetCursorStyle() { return CURSOR_STYLE; }
void Platform::SetCursorVisibility( bool visible ) {
    if( CURSOR_VISIBLE == visible ) {
        return;
    }
    ShowCursor( visible ? TRUE : FALSE );
    CURSOR_VISIBLE = visible;
}
bool Platform::IsCursorVisible() { return CURSOR_VISIBLE; }

void CenterCursor() {
    HWND window = GetActiveWindow();
    POINT screen = {};
    screen.x = WINDOW_WIDTH;
    screen.y = WINDOW_HEIGHT;
    ClientToScreen( window, &screen );
    SetCursorPos( screen.x, screen.y );

    CloseHandle( window );
}

void Platform::SetCursorLocked( bool lock ) {
    if( CURSOR_LOCKED == lock ) {
        return;
    }
    CenterCursor();
    CURSOR_LOCKED = lock;
}
bool Platform::IsCursorLocked() { return CURSOR_LOCKED; }

bool Platform::IsAppActive() { return APP_ACTIVE; }

bool WinLoadFileFromHandle( HANDLE fileHandle, Platform::File* result ) {
    LARGE_INTEGER fileSize;
    if( GetFileSizeEx( fileHandle, &fileSize ) == FALSE ) {
        LOG_WINDOWS_ERROR();
        result = {};
        CloseHandle( fileHandle );
        return false;
    }

    result->size = (usize)fileSize.QuadPart;
    result->data = Platform::Alloc( result->size );

    if( !result->data ) {
        LOG_ERROR("Windows x64 | LoadFile > Failed to allocate %llu bytes of memory!", result->size);
        result = {};
        CloseHandle( fileHandle );
        return false;
    }

    DWORD bytesToRead = result->size;
    DEBUG_ASSERT_LOG(result->size < (usize)U32::MAX, "Trying to read over 4GB!");
    // TODO(alicia): handle >=4GB case

    DWORD bytesRead;
    bool readResult = ReadFile(
        fileHandle,
        result->data,
        bytesToRead,
        &bytesRead,
        NULL
    ) && bytesRead == bytesToRead;
    if( !readResult ) {
        LOG_WINDOWS_ERROR();
        Platform::Free( result->data );
        result = {};
        CloseHandle( fileHandle );
        return false;
    }

    CloseHandle( fileHandle );
    LOG_INFO("Windows x64 > Successfully loaded \"%s\" from disk!", result->filePath);
    return true;
}

bool Platform::LoadFile( const char* filePath, File* result ) {
    usize filePathLen = strLen( filePath );
    if( filePathLen == 0 ) {
        result = {};
        LOG_WARN( "Windows x64 > Attempted to load file from empty path!" );
        return false;
    }
    result->filePathLen = filePathLen;
    result->filePath = (char*)Platform::Alloc(result->filePathLen);
    strCopy( filePath, result->filePathLen, result->filePath );

    wchar_t wfilePath[filePathLen];
    strTowstr( filePath, wfilePath, filePathLen );

    HANDLE fileHandle = CreateFile(
        wfilePath,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0, 0
    );

    if( fileHandle == INVALID_HANDLE_VALUE ) {
        LOG_WINDOWS_ERROR();
        result = {};
        return false;
    }

    return WinLoadFileFromHandle( fileHandle, result );
}

bool Platform::UserLoadFile( const char* dialogTitle, File* result ) {
    const usize MAX_FILE_NAME = 512;

    usize dialogTitleLen = strLen(dialogTitle);
    wchar_t wdialogTitle[dialogTitleLen];
    strTowstr( dialogTitle, wdialogTitle, dialogTitleLen );

    // buffer for file name
    wchar_t szFile[MAX_FILE_NAME];

    HWND window = GetActiveWindow();

    OPENFILENAME openFileName = {};
    openFileName.lStructSize     = sizeof(openFileName);
    openFileName.hwndOwner       = window;
    openFileName.lpstrFile       = szFile;
    openFileName.lpstrFile[0]    = '\0';
    openFileName.nMaxFile        = sizeof(szFile);
    openFileName.lpstrFilter     = L"All\0*.*\0Text\0*.TXT\0";
    openFileName.nFilterIndex    = 1;
    openFileName.lpstrFileTitle  = NULL;
    openFileName.nMaxFileTitle   = 0;
    openFileName.lpstrInitialDir = NULL;
    openFileName.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    openFileName.lpstrTitle      = wdialogTitle;

    if( GetOpenFileName( &openFileName ) == FALSE ) {
        result = {};
        return false;
    }

    HANDLE fileHandle = CreateFile(
        openFileName.lpstrFile,
        GENERIC_READ,
        FILE_SHARE_READ, // dwShareMode
        NULL, // lpSecurityAttributes
        OPEN_EXISTING, // dwCreationDisposition
        0, 0
    );

    usize fileNameLen = strLenW( openFileName.lpstrFile );
    char fileName[fileNameLen];
    wstrTostr( openFileName.lpstrFile, fileName, fileNameLen );
    result->filePath    = fileName;
    result->filePathLen = fileNameLen;

    bool success = WinLoadFileFromHandle( fileHandle, result );

    CloseHandle(window);

    return success;
}

void Platform::FreeFile( File* file ) {
    Platform::Free( file->data );
    Platform::Free( file->filePath );
    file = {};
}

bool Platform::WriteFile( const char* filePath, void* buffer, usize bufferSize, WriteFileType writeType ) {

    DWORD createDisposition;
    switch(writeType) {
        case WriteFileType::NOT_CREATED_FAIL: {
            // TODO(alicia): check if this is the right flag
            createDisposition = OPEN_EXISTING;
        } break;
        default: {
            createDisposition = CREATE_ALWAYS;
        } break;
    }

    usize filePathLen = strLen( filePath );
    if( filePathLen == 0 ) {
        LOG_WARN( "Windows x64 > Attempted to write file to empty path!" );
        return false;
    }
    wchar_t wfilePath[filePathLen];
    strTowstr( filePath, wfilePath, filePathLen );

    HANDLE fileHandle = CreateFile(
        wfilePath,
        GENERIC_WRITE,
        0, NULL,
        createDisposition,
        0, NULL
    );
    if( fileHandle == INVALID_HANDLE_VALUE ) {
        LOG_WINDOWS_ERROR();
        return false;
    }

    DWORD bytesToWrite = bufferSize;
    DEBUG_ASSERT_LOG(bufferSize < (usize)U32::MAX, "Trying to write over 4GB!");
    // TODO(alicia): handle >=4GB case

    DWORD bytesWritten;
    if( !::WriteFile( fileHandle, buffer, bytesToWrite, &bytesWritten, nullptr ) ) {
        LOG_WINDOWS_ERROR();
        CloseHandle( fileHandle );
        return false;
    }
    CloseHandle( fileHandle );

    if( bytesWritten != bytesToWrite ) {
        LOG_ERROR("Windows x64 > Failed to write %li bytes, wrote %li bytes instead?", bytesToWrite, bytesWritten);
        return false;
    }
    
    return true;
}

Platform::KeyCode VKCodeToKeyCode( u32 VKCode ) {
    using namespace Platform;
    switch( VKCode ) {
        case VK_ESCAPE:    return KeyCode::ESCAPE;
        case VK_BACK:      return KeyCode::BACKSPACE;
        case VK_TAB:       return KeyCode::TAB;
        case VK_RETURN:    return KeyCode::ENTER;
        case VK_SPACE:     return KeyCode::SPACE;
        case VK_OEM_7:     return KeyCode::QUOTE;
        case '0':          return KeyCode::KEY_0;
        case '1':          return KeyCode::KEY_1;
        case '2':          return KeyCode::KEY_2;
        case '3':          return KeyCode::KEY_3;
        case '4':          return KeyCode::KEY_4;
        case '5':          return KeyCode::KEY_5;
        case '6':          return KeyCode::KEY_6;
        case '7':          return KeyCode::KEY_7;
        case '8':          return KeyCode::KEY_8;
        case '9':          return KeyCode::KEY_9;
        case VK_OEM_1:     return KeyCode::COLON;
        case VK_OEM_COMMA: return KeyCode::ANGLE;
        case VK_OEM_2:     return KeyCode::QUESTION;
        case VK_OEM_4:     return KeyCode::LBRACKET;
        case VK_OEM_6:     return KeyCode::RBRACKET;
        case VK_OEM_3:     return KeyCode::TILDE;
        case 'A':          return KeyCode::A;
        case 'B':          return KeyCode::B;
        case 'C':          return KeyCode::C;
        case 'D':          return KeyCode::D;
        case 'E':          return KeyCode::E;
        case 'F':          return KeyCode::F;
        case 'G':          return KeyCode::G;
        case 'H':          return KeyCode::H;
        case 'I':          return KeyCode::I;
        case 'J':          return KeyCode::J;
        case 'K':          return KeyCode::K;
        case 'L':          return KeyCode::L;
        case 'M':          return KeyCode::M;
        case 'N':          return KeyCode::N;
        case 'O':          return KeyCode::O;
        case 'P':          return KeyCode::P;
        case 'Q':          return KeyCode::Q;
        case 'R':          return KeyCode::R;
        case 'S':          return KeyCode::S;
        case 'T':          return KeyCode::T;
        case 'U':          return KeyCode::U;
        case 'V':          return KeyCode::V;
        case 'W':          return KeyCode::W;
        case 'X':          return KeyCode::X;
        case 'Y':          return KeyCode::Y;
        case 'Z':          return KeyCode::Z;
        case VK_DELETE:    return KeyCode::KEY_DELETE;
        case VK_CAPITAL:   return KeyCode::CAPSLOCK;
        case VK_F1:        return KeyCode::F1;
        case VK_F2:        return KeyCode::F2;
        case VK_F3:        return KeyCode::F3;
        case VK_F4:        return KeyCode::F4;
        case VK_F5:        return KeyCode::F5;
        case VK_F6:        return KeyCode::F6;
        case VK_F7:        return KeyCode::F7;
        case VK_F8:        return KeyCode::F8;
        case VK_F9:        return KeyCode::F9;
        case VK_F10:       return KeyCode::F10;
        case VK_F11:       return KeyCode::F11;
        case VK_F12:       return KeyCode::F12;
        case VK_F13:       return KeyCode::F13;
        case VK_F14:       return KeyCode::F14;
        case VK_F15:       return KeyCode::F15;
        case VK_F16:       return KeyCode::F16;
        case VK_F17:       return KeyCode::F17;
        case VK_F18:       return KeyCode::F18;
        case VK_F19:       return KeyCode::F19;
        case VK_F20:       return KeyCode::F20;
        case VK_F21:       return KeyCode::F21;
        case VK_F22:       return KeyCode::F22;
        case VK_F23:       return KeyCode::F23;
        case VK_F24:       return KeyCode::F24;
        case VK_UP:        return KeyCode::UP;
        case VK_DOWN:      return KeyCode::DOWN;
        case VK_LEFT:      return KeyCode::LEFT;
        case VK_RIGHT:     return KeyCode::RIGHT;
        case VK_NUMPAD0:   return KeyCode::PAD_0;
        case VK_NUMPAD1:   return KeyCode::PAD_1;
        case VK_NUMPAD2:   return KeyCode::PAD_2;
        case VK_NUMPAD3:   return KeyCode::PAD_3;
        case VK_NUMPAD4:   return KeyCode::PAD_4;
        case VK_NUMPAD5:   return KeyCode::PAD_5;
        case VK_NUMPAD6:   return KeyCode::PAD_6;
        case VK_NUMPAD7:   return KeyCode::PAD_7;
        case VK_NUMPAD8:   return KeyCode::PAD_8;
        case VK_NUMPAD9:   return KeyCode::PAD_9;
        case VK_LWIN:      return KeyCode::LSUPER;
        case VK_RWIN:      return KeyCode::RSUPER;
        case VK_CONTROL:   return KeyCode::CTRL;
        case VK_SHIFT:     return KeyCode::SHIFT;
        case VK_MENU:      return KeyCode::ALT;
        default:           return KeyCode::UNKNOWN;
    }
}

#endif

