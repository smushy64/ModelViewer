#include "gl.hpp"
#include "glad/glad.h"
#include "alias.hpp"
#include "debug.hpp"

using namespace Platform;

void RendererOpenGL::ClearBuffer() {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void RendererOpenGL::SwapBuffer() {
    OpenGLSwapBufferFn();
}

bool RendererOpenGL::LoadOpenGLFunctions( OpenGLLoader loader ) {
    return gladLoadGLLoader( (GLADloadproc)loader ) != 0;
}

void RendererOpenGL::SetClearColor( const glm::vec4& clearColor ) {
    m_clearColor = clearColor;
    glClearColor( clearColor.r, clearColor.g, clearColor.b, clearColor.a );
}

void RendererOpenGL::SetViewport( const glm::vec2& viewport ) {
    m_viewport = viewport;
    glViewport( 0, 0, (GLsizei)viewport.x, (GLsizei)viewport.y );
}

#ifdef DEBUG
void OpenGLDebugMessageCallback(
    GLenum, // source
    GLenum, // type
    GLuint id,
    GLenum severity,
    GLsizei, // length
    const GLchar *message,
    const void* // userParam
);
#endif

bool RendererOpenGL::Initialize() {

#ifdef DEBUG
    glEnable( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( OpenGLDebugMessageCallback, nullptr );
#endif

    return true;
}

#ifdef DEBUG

const usize IGNORE_IDS[] = { 131185 };
const usize IGNORE_IDS_COUNT = 1;

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

void OpenGLDebugMessageCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei, // message length
    const GLchar *message,
    const void* // userParam
) {
    for( usize i = 0; i < IGNORE_IDS_COUNT; i++ ) {
        if( id == IGNORE_IDS[i] ) {
            return;
        }
    }

    switch(severity) {
        case GL_DEBUG_SEVERITY_HIGH: {
            LOG_ERROR("OpenGL [%i] > [SRC %s] [TYPE %s] %s",
                id,
                DebugSourceToString( source ),
                DebugTypeToString( type ),
                message
            );
        } break;
        case GL_DEBUG_SEVERITY_MEDIUM:
        case GL_DEBUG_SEVERITY_LOW: {
            LOG_WARN("OpenGL [%i] > [SRC %s] [TYPE %s] %s",
                id,
                DebugSourceToString( source ),
                DebugTypeToString( type ),
                message
            );
        } break;
        default: case GL_DEBUG_SEVERITY_NOTIFICATION: {
            LOG_INFO("OpenGL [%i] > [SRC %s] [TYPE %s] %s",
                id,
                DebugSourceToString( source ),
                DebugTypeToString( type ),
                message
            );
        } break;
    }
}

#endif
