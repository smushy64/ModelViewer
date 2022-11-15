/**
 * Description:  OpenGL
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 15, 2022 
*/
#pragma once
#include "pch.hpp"

namespace Platform {

typedef void* (*OpenGLLoadProc)(const char* functionName);

bool OpenGLLoadFunctions( OpenGLLoadProc loadProc );
void OpenGLInitialize();
void OpenGLClearBuffer();
void OpenGLSetClearColor( f32 r, f32 g, f32 b, f32 a );
void OpenGLSetViewport( i32 width, i32 height );

} // namespace Platform

