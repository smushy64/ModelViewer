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

    api->Initialize    = OpenGLInitialize;
    api->ClearBuffer   = OpenGLClearBuffer;
    api->SetClearColor = OpenGLSetClearColor;
    api->SetViewport   = OpenGLSetViewport;

    return true;
}
