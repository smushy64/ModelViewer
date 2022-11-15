/**
 * Description:  Renderer
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 15, 2022 
*/
#pragma once
#include "pch.hpp"

namespace Platform {

enum class RendererBackend {
    OPENGL
};

typedef void (*InitializeFN)();
typedef void (*ClearBufferFN)();
typedef void (*SwapBuffersFN)();
typedef void (*SetClearColorFN)( f32 r, f32 g, f32 b, f32 a );
typedef void (*SetViewportFN)( i32 width, i32 height );

typedef void* (*OpenGLLoadProc)(const char* functionName);

/// Structure containing function pointers to API calls  
struct RendererAPI {
    /// @brief Initialize API
    InitializeFN Initialize;
    /// @brief Clear Buffer
    ClearBufferFN ClearBuffer;
    /// @brief Swap Buffers
    ///  IMPORTANT: This function is supplied by the platform for OpenGL!!!
    SwapBuffersFN SwapBuffers;
    /// @brief Set color to clear buffers to
    /// @param r,g,b,a Clear color
    SetClearColorFN SetClearColor;
    /// @brief Set viewport dimensions
    /// @param width Width in pixels
    /// @param height Height in pixels
    SetViewportFN SetViewport;
};

bool CreateOpenGLAPI( RendererAPI* api, OpenGLLoadProc loadProc );

struct Renderer {
    RendererAPI api;
};

} // namespace Platform

