/**
 * Description:  OpenGL
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 15, 2022 
 */
#include "gl.hpp"
#include "glad/glad.h"
#include "platform/renderer.hpp"
#include "platform/io.hpp"
#include "util.hpp"
static bool DELETE_PROGRAMS_ARB = false;
static i32 EXT_COUNT = 0;
#define AUTO_MIPMAP 0
#define TEX_NO_BORDER 0

GLenum TextureWrapModeToGLenum( Platform::TextureWrapMode mode );
GLenum TextureMinFilterToGLenum( Platform::TextureMinFilter filter );
GLenum TextureMagFilterToGLenum( Platform::TextureMagFilter filter );
GLenum TextureFormatToGLenum( Platform::TextureFormat format );
GLenum TypeToGLenum( Platform::Type format );

void Platform::OpenGLSetTexture2DWrapMode( Texture2D* texture, TextureWrapMode wrapX, TextureWrapMode wrapY ) {
    glBindTexture( GL_TEXTURE_2D, texture->id );
    texture->wrapModeX = wrapX;
    texture->wrapModeY = wrapY;
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureWrapModeToGLenum( wrapX ) );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureWrapModeToGLenum( wrapY ) );
}
void Platform::OpenGLSetTexture2DFilter( Texture2D* texture, TextureMinFilter minFilter, TextureMagFilter magFilter ) {
    glBindTexture( GL_TEXTURE_2D, texture->id );
    texture->minFilter = minFilter;
    texture->magFilter = magFilter;
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureMinFilterToGLenum( minFilter ) );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureMagFilterToGLenum( magFilter ) );
}
void Platform::OpenGLUseTexture2D( Texture2D* texture, u32 unit ) {
    glBindTextureUnit( unit, texture->id );
}
Platform::Texture2D Platform::OpenGLCreateTexture2D(
    i32 width,
    i32 height,
    u8* data,
    TextureFormat format,
    Type dataType,
    TextureWrapMode wrapX,
    TextureWrapMode wrapY,
    TextureMinFilter minFilter,
    TextureMagFilter magFilter
) {
    Texture2D result = {};
    result.data      = data;
    result.width     = width;
    result.height    = height;
    result.format    = format;
    result.dataType  = dataType;

    glGenTextures( 1, &result.id );
    glBindTexture( GL_TEXTURE_2D, result.id );
    glTexImage2D(
        GL_TEXTURE_2D,
        AUTO_MIPMAP,
        TextureFormatToGLenum( result.format ),
        result.width, result.height,
        TEX_NO_BORDER,
        TextureFormatToGLenum(result.format),
        TypeToGLenum( result.dataType ),
        result.data
    );
    glGenerateMipmap( GL_TEXTURE_2D );

    Platform::OpenGLSetTexture2DWrapMode( &result, wrapX, wrapY );
    Platform::OpenGLSetTexture2DFilter( &result, minFilter, magFilter );

    return result;
}
void Platform::OpenGLDeleteTextures2D( Texture2D* textures, usize textureCount ) {
    GLuint textureIDs[textureCount];
    forloop( (isize)textureCount ) {
        textureIDs[i] = textures[i].id;
    }
    glDeleteTextures( textureCount, textureIDs );
}

void Platform::OpenGLUniformFloat( Shader* shader, i32 uniform, f32 value ) {
    glProgramUniform1f( shader->id, uniform, value );
}
void Platform::OpenGLUniformUInt( Shader* shader, i32 uniform, u32 value ) {
    glProgramUniform1ui( shader->id, uniform, value );
}
void Platform::OpenGLUniformInt( Shader* shader, i32 uniform, i32 value ) {
    glProgramUniform1i( shader->id, uniform, value );
}
bool Platform::OpenGLGetUniformID( Shader* shader, const char* uniformName, i32* result ) {
    *result = glGetUniformLocation( shader->id, uniformName );
    if( *result < 0 ) {
        LOG_ERROR("OpenGL > Uniform \"%s\" could not be found!", uniformName );
        return false;
    } else {
        return true;
    }
}
void Platform::OpenGLUseShader( Shader* shader ) {
    glUseProgram( shader->id );
}
void Platform::OpenGLDeleteShaders( Shader* shaders, usize shaderCount ) {
    u32 ids[shaderCount];
    forloop((isize)shaderCount) {
        ids[i] = shaders[i].id;
    }
    if(DELETE_PROGRAMS_ARB) {
        glDeleteProgramsARB( shaderCount, ids );
    } else {
        forloop( (isize)shaderCount ) {
            glDeleteProgram( ids[i] );
        }
    }
}
bool CompileShader( const char* source, usize sourceLen, GLenum shaderType, GLuint* shaderID ) {
    *shaderID = glCreateShader( shaderType );
    i32 sourceLeni32 = (i32)sourceLen;
    DEBUG_ASSERT_LOG( sourceLen == (usize)sourceLeni32, "OpenGL > Shader Source is >4GB?!" );
    glShaderSource(
        *shaderID,
        1,
        &source,
        &sourceLeni32
    );
    glCompileShader( *shaderID );

    i32 success;
    glGetShaderiv( *shaderID, GL_COMPILE_STATUS, &success );
    if( success == GL_FALSE ) {
        i32 len = 0;
        glGetShaderiv( *shaderID, GL_INFO_LOG_LENGTH, &len );
        char buffer[len];
        glGetShaderInfoLog(
            *shaderID,
            len,
            nullptr,
            buffer
        );
        LOG_ERROR("OpenGL Compilation Error > %s", buffer);
        return false;
    } else { return true; }
}
bool LinkShaders( GLuint shaders[], usize count, GLuint* programID ) {
    *programID = glCreateProgram();
    for( usize i = 0; i < count; i++ ) {
        glAttachShader( *programID, shaders[i] );
    }
    glLinkProgram( *programID );
    for( usize i = 0; i < count; i++ ) {
        glDetachShader( *programID, shaders[i] );
        glDeleteShader( shaders[i] );
    }

    i32 success;
    glGetProgramiv( *programID, GL_LINK_STATUS, &success );
    if( success == GL_FALSE ) {
        i32 len = 0;
        glGetProgramiv( *programID, GL_INFO_LOG_LENGTH, &len );
        char buffer[len];
        glGetProgramInfoLog(
            *programID,
            len,
            nullptr,
            buffer
        );
        LOG_ERROR("OpenGL Linking Error > %s", buffer);
        return false;
    } else { return true; }

}
bool Platform::OpenGLCreateShader(
    const char* vertexSrc,
    usize vertexLen,
    const char* fragmentSrc,
    usize fragmentLen,
    Shader* result
) {
    *result = {};

    GLuint vertex = 0, frag = 0;
    if(!CompileShader( vertexSrc, vertexLen, GL_VERTEX_SHADER, &vertex )) {
        return false;
    }
    if(!CompileShader( fragmentSrc, fragmentLen, GL_FRAGMENT_SHADER, &frag )) {
        return false;
    }

    GLuint program = 0;
    GLuint shaders[] = { vertex, frag };
    if(!LinkShaders( shaders, 2, &program )) {
        return false;
    }

    result->id = program;
    return true;
}

bool Platform::OpenGLLoadFunctions( OpenGLLoadProc loadProc ) {
    bool result = gladLoadGLLoader( loadProc ) != 0;
    return result;
}
void Platform::OpenGLClearBuffer() {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
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
    glEnable( GL_CULL_FACE );

    // Append to window title " | OpenGL *version* "
    const char* version = (const char*)glGetString( GL_VERSION );
    LOG_INFO("OpenGL Version: %s", version);
    usize versionLen = strLen(version);
    const char* titleAppend = " | OpenGL ";
    usize titleAppendLen = strLen( titleAppend );
    usize newTitleAppendLen = versionLen + titleAppendLen;
    char newTitleAppend[newTitleAppendLen];
    strConcat(
        titleAppendLen, titleAppend,
        versionLen, version,
        newTitleAppendLen, newTitleAppend
    );
    Platform::AppendToWindowTitle( newTitleAppend, newTitleAppendLen );

#if DEBUG
    const char* vendor = (const char*)glGetString( GL_VENDOR );
    LOG_INFO("OpenGL Vendor: %s", vendor);
    const char* renderer = (const char*)glGetString( GL_RENDERER );
    LOG_INFO("OpenGL Renderer: %s", renderer);
    const char* glslVersion = (const char*)glGetString( GL_SHADING_LANGUAGE_VERSION );
    LOG_INFO("OpenGL GLSL Version: %s", glslVersion);

    // Check for extensions
    glGetIntegerv( GL_NUM_EXTENSIONS, &EXT_COUNT );
    LOG_INFO("OpenGL extensions: %i", EXT_COUNT);
#endif

    if( glDeleteProgramsARB ) {
        DELETE_PROGRAMS_ARB = true;
    }

}

GLenum TextureWrapModeToGLenum( Platform::TextureWrapMode mode ) {
    using namespace Platform;
    switch( mode ) {
        case TextureWrapMode::CLAMP: return GL_CLAMP_TO_EDGE;
        case TextureWrapMode::REPEAT: return GL_REPEAT;
        case TextureWrapMode::MIRROR_CLAMP: return GL_MIRROR_CLAMP_TO_EDGE;
        case TextureWrapMode::MIRROR_REPEAT: return GL_MIRRORED_REPEAT;
        default: return GL_INVALID_ENUM;
    }
}
GLenum TextureMinFilterToGLenum( Platform::TextureMinFilter filter ) {
    using namespace Platform;
    switch( filter ) {
        case TextureMinFilter::LINEAR: return GL_LINEAR;
        case TextureMinFilter::NEAREST: return GL_NEAREST;
        case TextureMinFilter::LINEAR_MIPMAP_LINEAR: return GL_LINEAR_MIPMAP_LINEAR;
        case TextureMinFilter::LINEAR_MIPMAP_NEAREST: return GL_LINEAR_MIPMAP_NEAREST;
        case TextureMinFilter::NEAREST_MIPMAP_LINEAR: return GL_NEAREST_MIPMAP_LINEAR;
        case TextureMinFilter::NEAREST_MIPMAP_NEAREST: return GL_NEAREST_MIPMAP_NEAREST;
        default: return GL_INVALID_ENUM;
    }
}
GLenum TextureMagFilterToGLenum( Platform::TextureMagFilter filter ) {
    using namespace Platform;
    switch( filter ) {
        case TextureMagFilter::LINEAR: return GL_LINEAR;
        case TextureMagFilter::NEAREST: return GL_NEAREST;
        default: return GL_INVALID_ENUM;
    }
}
GLenum TextureFormatToGLenum( Platform::TextureFormat format ) {
    using namespace Platform;
    switch( format ) {
        case TextureFormat::R:    return GL_RED;
        case TextureFormat::RG:   return GL_RG;
        case TextureFormat::RGB:  return GL_RGB;
        case TextureFormat::RGBA: return GL_RGBA;
        default: return GL_INVALID_ENUM;
    }
}
GLenum TypeToGLenum( Platform::Type format ) {
    using namespace Platform;
    switch( format ) {
        case Type::UNSIGNED_BYTE:  return GL_UNSIGNED_BYTE;
        case Type::BYTE:           return GL_BYTE;
        case Type::UNSIGNED_SHORT: return GL_UNSIGNED_SHORT;
        case Type::SHORT:          return GL_SHORT;
        case Type::UNSIGNED_INT:   return GL_UNSIGNED_INT;
        case Type::INT:            return GL_INT;
        case Type::FLOAT:          return GL_FLOAT;
        case Type::DOUBLE:         return GL_DOUBLE;
        default: return GL_INVALID_ENUM;
    }
}

