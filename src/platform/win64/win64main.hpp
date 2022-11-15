/**
 * Description:  Windows x64 main
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 12, 2022 
*/
#if WINDOWS
#pragma once

#include "pch.hpp"
#include <windows.h>

// forward declaration
namespace Core {
    struct AppContext;
}

#if __GNUG__
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

/// @brief Allocate memory in the heap
/// @param size amount to alloc
/// @return Pointer to memory
void* WinAlloc( usize size );
/// @brief Free memory in the heap
/// @param mem pointer to memory
void WinFree( void* mem );
/// @brief Format windows error
/// @param buffer buffer to store error into
/// @param bufferSize size of the buffer
/// @param errorCode pointer to DWORD to store error code
void WinFormatError( char* buffer, usize bufferSize, LPDWORD errorCode );
/// @brief Get real-time
/// @return ticks
u64 WinGetTime();

/// @brief Log windows error to the console
#define LOG_WINDOWS_ERROR() do {\
    char buffer[512];\
    DWORD errorCode = 0;\
    WinFormatError( buffer, 512, &errorCode );\
    LOG_ERROR("Windows x64(%li) > %s", errorCode, buffer );\
} while(0)
#define SUCCESS_RETURN_CODE 0
#define ERROR_RETURN_CODE -1

HWND WinCreateWindow(
    HINSTANCE hInstance,
    const char* windowTitle,
    i32 width, i32 height,
    Core::AppContext* appContext
);
LRESULT WinWindowProc(
    HWND window,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
);
void WinProcessMessages(
    HWND window,
    Core::AppContext* appContext
);
HGLRC WinCreateOpenGLContext( HDC deviceContext );

namespace wglFN {
    typedef HGLRC (*wglCreateContextAttribsARBFN)( HDC hDc, HGLRC hShareContext, const i32* attribList );
    typedef HGLRC (*wglCreateContextFN)( HDC unnamedParam1 );
    typedef BOOL (*wglMakeCurrentFN)( HDC unnamedParam1, HGLRC unnamedParam2 );
    typedef BOOL (*wglDeleteContextFN)( HGLRC unnamedParam1 );
    typedef PROC (*wglGetProcAddressFN)( LPCSTR unnamedParam1 );

    wglCreateContextAttribsARBFN inWGLCreatewglCreateContextAttribsARB;
    wglCreateContextFN           inWGLCreateContext;
    wglMakeCurrentFN             inWGLMakeCurrent;
    wglDeleteContextFN           inWGLDeleteContext;
    wglGetProcAddressFN          inWGLGetProcAddress;
}

#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB               0x2093
#define WGL_CONTEXT_FLAGS_ARB                     0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126
#define WGL_CONTEXT_DEBUG_BIT_ARB                 0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB    0x0002
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define ERROR_INVALID_VERSION_ARB                 0x2095
#define ERROR_INVALID_PROFILE_ARB                 0x2096

void* WinOpenGLLoadProc( const char* functionName );
bool LoadWGLFunctions( HMODULE openglModule );

/// @brief The wglCreateContext function creates a new OpenGL rendering context, which is suitable for drawing on the device referenced by hdc. The rendering context has the same pixel format as the device context.
/// @param unnamedParam1 [HDC] Handle to a device context for which the function creates a suitable OpenGL rendering context.
/// @return [HGLRC] If the function succeeds, the return value is a valid handle to an OpenGL rendering context.
/// If the function fails, the return value is NULL. To get extended error information, call GetLastError.
#define wglCreateContext wglFN::inWGLCreateContext

/// @brief The wglCreateContextAttribsARB function creates a new OpenGL >=3.2 rendering context.
/// @param hDc [HDC] Handle to a device context for which the function creates a suitable OpenGL rendering context.
/// @param hShareContext [HGLRC] Context to share OpenGL data with.
/// @param attribList [const i32*] List of attributes terminated with a 0
/// @return [HGLRC] If the function succeeds, the return value is a valid handle to an OpenGL rendering context.
/// If the function fails, the return value is NULL. To get extended error information, call GetLastError.
#define wglCreateContextAttribsARB wglFN::inWGLCreatewglCreateContextAttribsARB

/// @brief The wglMakeCurrent function makes a specified OpenGL rendering context the calling thread's current rendering context. All subsequent OpenGL calls made by the thread are drawn on the device identified by hdc. You can also use wglMakeCurrent to change the calling thread's current rendering context so it's no longer current.
/// @param unnamedParam1 [HDC] Handle to a device context. Subsequent OpenGL calls made by the calling thread are drawn on the device identified by hdc.
/// @param unnamedParam2 [HGLRC] Handle to an OpenGL rendering context that the function sets as the calling thread's rendering context. If hglrc is NULL, the function makes the calling thread's current rendering context no longer current, and releases the device context that is used by the rendering context. In this case, hdc is ignored.
/// @return [BOOL] When the wglMakeCurrent function succeeds, the return value is TRUE; otherwise the return value is FALSE. To get extended error information, call GetLastError.
#define wglMakeCurrent wglFN::inWGLMakeCurrent

/// @brief The wglDeleteContext function deletes a specified OpenGL rendering context.
/// @param unnamedParam1 [HGLRC] Handle to an OpenGL rendering context that the function will delete.
/// @return [BOOL] If the function succeeds, the return value is TRUE. If the function fails, the return value is FALSE. To get extended error information, call GetLastError.
#define wglDeleteContext wglFN::inWGLDeleteContext

/// @brief The wglGetProcAddress function returns the address of an OpenGL extension function for use with the current OpenGL rendering context.
/// @param unnamedParam1 [LPCSTR] Points to a null-terminated string that is the name of the extension function. The name of the extension function must be identical to a corresponding function implemented by OpenGL.
/// @return [PROC] When the function succeeds, the return value is the address of the extension function.
/// When no current rendering context exists or the function fails, the return value is NULL. To get extended error information, call GetLastError.
#define wglGetProcAddress wglFN::inWGLGetProcAddress

#endif