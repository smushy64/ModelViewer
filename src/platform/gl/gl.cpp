/**
 * Description:  OpenGL
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 15, 2022 
 */
#include "gl.hpp"
#include "glad/glad.h"

bool Platform::OpenGLLoadFunctions( OpenGLLoadProc loadProc ) {
    bool result = gladLoadGLLoader( loadProc ) != 0;
    return result;
}
void Platform::OpenGLClearBuffer() {
    glClear( GL_COLOR_BUFFER_BIT );
}
void Platform::OpenGLSetClearColor( f32 r, f32 g, f32 b, f32 a ) {
    glClearColor( r, g, b, a );
}
void Platform::OpenGLSetViewport( i32 width, i32 height ) {
    glViewport( 0, 0, width, height );
}

const char* DebugSourceToString( GLenum source ) {
    switch(source) {
        case GL_DEBUG_SOURCE_API: return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "Window System";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
        case GL_DEBUG_SOURCE_THIRD_PARTY: return "3rd Party";
        case GL_DEBUG_SOURCE_APPLICATION: return "Application";
        default: return "Other";
    }
}

const char* DebugTypeToString( GLenum type ) {
    switch( type ) {
        case GL_DEBUG_TYPE_ERROR: return "Error";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated Behaviour";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "Undefined Behaviour";
        case GL_DEBUG_TYPE_PORTABILITY: return "Portability";
        case GL_DEBUG_TYPE_PERFORMANCE: return "Performance";
        case GL_DEBUG_TYPE_MARKER: return "Marker";
        case GL_DEBUG_TYPE_PUSH_GROUP: return "Push";
        case GL_DEBUG_TYPE_POP_GROUP: return "Pop";
        default: return "Other";
    }
}

void OpenGLDebugMessageCallback (
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei, // message length
    const GLchar *message,
    const void* // userParam
) {
    switch(severity) {
        case GL_DEBUG_SEVERITY_HIGH: {
            LOG_ERROR("OpenGL(%i) > [SRC %s] [TYPE %s] %s",
                id,
                DebugSourceToString( source ),
                DebugTypeToString( type ),
                message
            );
        } break;
        case GL_DEBUG_SEVERITY_MEDIUM:
        case GL_DEBUG_SEVERITY_LOW: {
            LOG_WARN("OpenGL(%i) > [SRC %s] [TYPE %s] %s",
                id,
                DebugSourceToString( source ),
                DebugTypeToString( type ),
                message
            );
        } break;
        default: case GL_DEBUG_SEVERITY_NOTIFICATION: {
            LOG_INFO("OpenGL(%i) > [SRC %s] [TYPE %s] %s",
                id,
                DebugSourceToString( source ),
                DebugTypeToString( type ),
                message
            );
        } break;
    }
}

void Platform::OpenGLInitialize() {
#if DEBUG
    glEnable( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( OpenGLDebugMessageCallback, nullptr );
#endif

    glEnable( GL_DEPTH_TEST );
}
