/**
 * Description:  Renderer
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 15, 2022 
*/
#include "renderer.hpp"
#include "gl/gl.hpp"

using namespace Platform;

bool Platform::CreateOpenGLAPI( RendererAPI* api, OpenGLLoadProc loadProc ) {
    if( !OpenGLLoadFunctions( loadProc ) ) {
        return false;
    }

    api->Initialize         = OpenGLInitialize;
    api->ClearBuffer        = OpenGLClearBuffer;
    api->SetClearColor      = OpenGLSetClearColor;
    api->SetViewport        = OpenGLSetViewport;

    api->CreateShader       = OpenGLCreateShader;
    api->DeleteShaders      = OpenGLDeleteShaders;
    api->UseShader          = OpenGLUseShader;
    api->GetUniformID       = OpenGLGetUniformID;
    api->UniformFloat       = OpenGLUniformFloat;
    api->UniformUInt        = OpenGLUniformUInt;
    api->UniformInt         = OpenGLUniformInt;

    api->CreateTexture2D      = OpenGLCreateTexture2D;
    api->UseTexture2D         = OpenGLUseTexture2D;
    api->SetTexture2DFilter   = OpenGLSetTexture2DFilter;
    api->SetTexture2DWrapMode = OpenGLSetTexture2DWrapMode;

    return true;
}
