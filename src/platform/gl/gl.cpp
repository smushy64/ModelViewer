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
GLenum InternalTextureFormatToGLenum( Platform::TextureFormat format );
GLenum DataTypeToGLenum( Platform::DataType format );
GLenum BlendFactorToGLenum( Platform::BlendFactor factor );
GLenum BlendEqToGLenum( Platform::BlendEq eq );

void Platform::OpenGLSetWireframeEnabled( bool enabled ) {
    glPolygonMode( GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL );
}

void Platform::OpenGLDrawVertexArray( VertexArray* vertexArray ) {
    if( vertexArray->indexBuffer ) {
        glDrawElements(
            GL_TRIANGLES,
            vertexArray->indexBuffer->indexCount,
            DataTypeToGLenum( vertexArray->indexBuffer->dataType ),
            nullptr
        );
    } else {
        glDrawArrays(
            GL_TRIANGLES,
            0,
            vertexArray->totalVertexCount
        );
    }
}

void Platform::OpenGLDeleteBuffers( usize bufferCount, u32* bufferIDs ) {
    glDeleteBuffers( bufferCount, bufferIDs );
}

Platform::IndexBuffer Platform::OpenGLCreateIndexBuffer( usize indexCount, void* indices, DataType indexDataType ) {
    DEBUG_ASSERT_LOG(
        indexDataType == DataType::UNSIGNED_BYTE ||
        indexDataType == DataType::UNSIGNED_SHORT ||
        indexDataType == DataType::UNSIGNED_INT,
        "Index Data type can only be unsigned byte, short or int! Data type given: %s",
        Platform::DataTypeToString( indexDataType )
    );

    IndexBuffer result = {};
    result.dataType   = indexDataType;
    result.indexCount = indexCount;
    result.bufferSize = result.indexCount * Platform::DataTypeSize( result.dataType );
    result.indices    = Platform::Alloc( result.bufferSize );

    Platform::MemCopy( result.bufferSize, indices, result.indices );

    glGenBuffers( 1, &result.id );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, result.id );
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        (GLsizeiptr)result.bufferSize,
        result.indices,
        GL_STATIC_DRAW // TODO(alicia): usage?
    );
    
    return result;
}
void Platform::OpenGLUseIndexBuffer( IndexBuffer* buffer ) {
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffer->id );
}
void Platform::OpenGLDeleteIndexBuffers( usize count, IndexBuffer* buffers ) {
    u32 indexBufferIDs[count];
    ucycles( count ) {
        indexBufferIDs[i] = buffers[i].id;
        if( buffers[i].indices ) {
            Platform::Free( buffers[i].indices );
        }
    }
    glDeleteBuffers( count, indexBufferIDs );
}

Platform::VertexBuffer Platform::OpenGLCreateVertexBuffer( usize bufferSize, void* vertices, VertexBufferLayout layout ) {
    VertexBuffer result = {};
    result.layout       = layout;
    result.bufferSize   = bufferSize;
    result.vertices     = Platform::Alloc( result.bufferSize );
    result.vertexCount  = result.bufferSize / result.layout.stride;

    Platform::MemCopy( result.bufferSize, vertices, result.vertices );

    glGenBuffers( 1, &result.id );
    glBindBuffer( GL_ARRAY_BUFFER, result.id );
    glBufferData(
        GL_ARRAY_BUFFER,
        (GLsizeiptr)result.bufferSize,
        vertices,
        GL_STATIC_DRAW
    );

    ucycles( result.layout.elementCount ) {
        glVertexAttribPointer(
            i,
            Platform::DataStructureCount( result.layout.elements[i].structure ),
            DataTypeToGLenum( result.layout.elements[i].dataType ),
            result.layout.elements[i].normalized ? GL_TRUE : GL_FALSE,
            result.layout.stride,
            (const void*)result.layout.elementOffsets[i]
        );
        glEnableVertexAttribArray(i);
    }

    return result;
}
void Platform::OpenGLUseVertexBuffer( VertexBuffer* buffer ) {
    glBindBuffer( GL_ARRAY_BUFFER, buffer->id );
}
void Platform::OpenGLDeleteVertexBuffers( usize count, VertexBuffer* buffers ) {
    u32 vertexBufferIDs[count];
    ucycles( count ) {
        vertexBufferIDs[i] = buffers[i].id;
        if( buffers[i].vertices ) {
            Platform::Free( buffers[i].vertices );
        }
        Platform::FreeVertexBufferLayout( &buffers[i].layout );
    }
    glDeleteBuffers( count, vertexBufferIDs );
}

Platform::VertexArray Platform::OpenGLCreateVertexArray() {
    VertexArray result = {};
    result.vertexBufferCount = 0;
    glGenVertexArrays( 1, &result.id );
    return result;
}
void Platform::OpenGLDeleteVertexArrays( usize count, VertexArray* vertexArrays ) {
    // collect buffer ids from vertex arrays and free memory
    // collect vertex array ids

    u32 vertexArrayIDs[count];
    usize bufferCount = 0;
    ucycles( count ) {
        vertexArrayIDs[i] = vertexArrays[i].id;
        bufferCount += vertexArrays[i].vertexBufferCount;
        if( vertexArrays[i].indexBuffer ) {
            bufferCount++;
        }
    }

    u32 bufferIDs[bufferCount];
    usize bufferIndex = 0;
    ucycles( count ) {
        if( vertexArrays[i].buffers ) {
            ucyclesi( vertexArrays[i].vertexBufferCount, j ) {
                bufferIDs[bufferIndex] = vertexArrays[i].buffers[j].id;
                Platform::Free( vertexArrays[i].buffers[j].vertices );
                Platform::FreeVertexBufferLayout( &vertexArrays[i].buffers[j].layout );
                bufferIndex++;
            }
        }
        if( vertexArrays[i].indexBuffer ) {
            bufferIDs[bufferIndex] = vertexArrays[i].indexBuffer->id;
            Platform::Free( vertexArrays[i].indexBuffer->indices );
            bufferIndex++;
        }
    }
    glDeleteBuffers( bufferCount, bufferIDs );
    glDeleteVertexArrays( count, vertexArrayIDs );
}
void Platform::OpenGLUseVertexArray( VertexArray* vertexArray ) {
    glBindVertexArray( vertexArray->id );
}
void Platform::OpenGLVertexArrayBindVertexBuffer( VertexArray* vertexArray, VertexBuffer buffer ) {
    // NOTE(alicia): This is probably the least performant way to do this
    // but considering this project is for learning/showing my programming skills
    // it's ok :)
    
    // if vertex array already has buffers
    if( vertexArray->vertexBufferCount > 0 ) {
        vertexArray->totalVertexCount += buffer.vertexCount;
        usize previousCount = vertexArray->vertexBufferCount;
        vertexArray->vertexBufferCount++;
        VertexBuffer temp[vertexArray->vertexBufferCount];
        Platform::MemCopy(
            sizeof(VertexBuffer) * previousCount,
            vertexArray->buffers,
            temp
        );
        Platform::Free( vertexArray->buffers );
        Platform::MemCopy( sizeof(VertexBuffer), &buffer, &temp[previousCount] );
        usize newSize = vertexArray->vertexBufferCount * sizeof(VertexBuffer);
        vertexArray->buffers = (VertexBuffer*)Platform::Alloc( newSize );
        Platform::MemCopy( newSize, temp, vertexArray->buffers );
    }
    // if vertex array does not yet have any buffers
    else {
        vertexArray->totalVertexCount = buffer.vertexCount;
        vertexArray->buffers = (VertexBuffer*)Platform::Alloc( sizeof(VertexBuffer) );
        Platform::MemCopy( sizeof(VertexBuffer), &buffer, vertexArray->buffers );
        vertexArray->vertexBufferCount = 1;
    }
}
void Platform::OpenGLVertexArrayBindIndexBuffer( VertexArray* vertexArray, IndexBuffer buffer ) {
    if( vertexArray->indexBuffer ) {
        Platform::OpenGLDeleteIndexBuffers( 1, vertexArray->indexBuffer );
        Platform::Free( vertexArray->indexBuffer );
    }

    vertexArray->indexBuffer = (IndexBuffer*)Platform::Alloc( sizeof(IndexBuffer) );
    Platform::MemCopy( sizeof(IndexBuffer), &buffer, vertexArray->indexBuffer );
}

Platform::UniformBuffer Platform::OpenGLCreateUniformBuffer( usize size, void* data ) {
    UniformBuffer result = {};
    result.size = size;
    glGenBuffers( 1, &result.id );
    glBindBuffer( GL_UNIFORM_BUFFER, result.id );
    glBufferData(
        GL_UNIFORM_BUFFER,
        (GLsizeiptr)result.size,
        data,
        GL_STATIC_DRAW // TODO: usage
    );
    return result;
}
void Platform::OpenGLDeleteUniformBuffers( usize bufferCount, UniformBuffer* buffers ) {
    GLuint uniformBufferIDs[bufferCount];
    ucycles( bufferCount ) {
        uniformBufferIDs[i] = buffers[i].id;
    }
    glDeleteBuffers( bufferCount, uniformBufferIDs );
}
void Platform::OpenGLUniformBufferData(UniformBuffer* uniformBuffer, usize size, void* data) {
    DEBUG_ASSERT_LOG( size == uniformBuffer->size,
        "OpenGL | UniformBufferData > Uniform Buffer size(%llu) does not match input size(%llu)!",
        uniformBuffer->size, size
    );
    glBindBuffer( GL_UNIFORM_BUFFER, uniformBuffer->id );
    glBufferData(
        GL_UNIFORM_BUFFER,
        (GLsizeiptr)size,
        data,
        GL_STATIC_DRAW // TODO: usage
    );
}
void Platform::OpenGLUniformBufferSubData(UniformBuffer* uniformBuffer, usize offset, usize size, void* data) {
    DEBUG_ASSERT_LOG( offset < uniformBuffer->size,
        "OpenGL | UniformBufferSubData > Offset(%llu) is greater than Uniform Buffer size(%llu)!",
        offset, uniformBuffer->size
    );
    DEBUG_ASSERT_LOG( size < uniformBuffer->size,
        "OpenGL | UniformBufferSubData > Size(%llu) is greater than Uniform Buffer size(%llu)!",
        size, uniformBuffer->size
    );
    DEBUG_ASSERT_LOG( offset + size <= uniformBuffer->size,
        "OpenGL | UniformBufferSubData > Offset + Size (%llu) is greater than Uniform Buffer size(%llu)!",
        offset + size, uniformBuffer->size
    );

    glBindBuffer( GL_UNIFORM_BUFFER, uniformBuffer->id );
    glBufferSubData(
        GL_UNIFORM_BUFFER,
        (GLintptr)offset,
        (GLsizeiptr)size,
        data
    );
}
void Platform::OpenGLUniformBufferSetBindingPoint(UniformBuffer* uniformBuffer, u32 bindingPoint) {
    glBindBufferBase( GL_UNIFORM_BUFFER, bindingPoint, uniformBuffer->id );
}
void Platform::OpenGLUniformBufferSetBindingPointRange(UniformBuffer* uniformBuffer, u32 bindingPoint, usize offset, usize size) {
    DEBUG_ASSERT_LOG( offset < uniformBuffer->size,
        "OpenGL | UniformBufferSetBindingPointRange > Offset(%llu) is greater than Uniform Buffer size(%llu)!",
        offset, uniformBuffer->size
    );
    DEBUG_ASSERT_LOG( size < uniformBuffer->size,
        "OpenGL | UniformBufferSetBindingPointRange > Size(%llu) is greater than Uniform Buffer size(%llu)!",
        size, uniformBuffer->size
    );
    DEBUG_ASSERT_LOG( offset + size <= uniformBuffer->size,
        "OpenGL | UniformBufferSetBindingPointRange > Offset + Size (%llu) is greater than Uniform Buffer size(%llu)!",
        offset + size, uniformBuffer->size
    );
    glBindBufferRange(
        GL_UNIFORM_BUFFER,
        bindingPoint,
        uniformBuffer->id,
        offset,
        size
    );
}

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
    void* data,
    TextureFormat format,
    DataType dataType,
    TextureWrapMode wrapX,
    TextureWrapMode wrapY,
    TextureMinFilter minFilter,
    TextureMagFilter magFilter
) {
    Texture2D result = {};
    result.width     = width;
    result.height    = height;
    result.dataSize  = result.width * result.height * DataTypeSize( dataType ) * TextureFormatComponentCount( format );
    result.data      = (u8*)Platform::Alloc( result.dataSize );
    Platform::MemCopy( result.dataSize, data, result.data );
    result.format    = format;
    result.dataType  = dataType;

    glGenTextures( 1, &result.id );
    glBindTexture( GL_TEXTURE_2D, result.id );

    Platform::OpenGLSetTexture2DWrapMode( &result, wrapX, wrapY );
    Platform::OpenGLSetTexture2DFilter( &result, minFilter, magFilter );

    glTexImage2D(
        GL_TEXTURE_2D,
        AUTO_MIPMAP,
        InternalTextureFormatToGLenum( result.format ),
        result.width, result.height,
        TEX_NO_BORDER,
        TextureFormatToGLenum(result.format),
        DataTypeToGLenum( result.dataType ),
        result.data
    );
    glGenerateMipmap( GL_TEXTURE_2D );

    return result;
}
void Platform::OpenGLDeleteTextures2D( usize textureCount, Texture2D* textures ) {
    GLuint textureIDs[textureCount];
    ucycles( textureCount ) {
        textureIDs[i] = textures[i].id;
        if( textures[i].data ) {
            Platform::Free( textures[i].data );
        }
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
void Platform::OpenGLUniformVec2( Shader* shader, i32 uniform, smath::vec2* value ) {
    glProgramUniform2fv( shader->id, uniform, 1, value->valuePtr() );
}
void Platform::OpenGLUniformVec3( Shader* shader, i32 uniform, smath::vec3* value ) {
    glProgramUniform3fv( shader->id, uniform, 1, value->valuePtr() );
}
void Platform::OpenGLUniformVec4( Shader* shader, i32 uniform, smath::vec4* value ) {
    glProgramUniform4fv( shader->id, uniform, 1, value->valuePtr() );
}
void Platform::OpenGLUniformMat3( Shader* shader, i32 uniform, smath::mat3* value ) {
    glProgramUniformMatrix3fv( shader->id, uniform, 1, GL_FALSE, value->valuePtr() );
}
void Platform::OpenGLUniformMat4( Shader* shader, i32 uniform, smath::mat4* value ) {
    glProgramUniformMatrix4fv( shader->id, uniform, 1, GL_FALSE, value->valuePtr() );
}
bool Platform::OpenGLGetUniformID( Shader* shader, const char* uniformName, i32* result ) {
    i32 id = 0;
    id = glGetUniformLocation( shader->id, uniformName );
    if( id < 0 ) {
        LOG_ERROR("OpenGL > Uniform \"%s\" could not be found!", uniformName );
        return false;
    } else {
        *result = id;
        return true;
    }
}
void Platform::OpenGLUseShader( Shader* shader ) {
    glUseProgram( shader->id );
}
void Platform::OpenGLDeleteShaders( usize shaderCount, Shader* shaders ) {
    u32 ids[shaderCount];
    ucycles(shaderCount) {
        ids[i] = shaders[i].id;
    }
    if(DELETE_PROGRAMS_ARB) {
        glDeleteProgramsARB( shaderCount, ids );
    } else {
        ucycles( shaderCount ) {
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
void Platform::OpenGLSetPackAlignment( i32 packAlignment ) {
    glPixelStorei( GL_PACK_ALIGNMENT, packAlignment );
}
void Platform::OpenGLSetUnPackAlignment( i32 unpackAlignment ) {
    glPixelStorei( GL_UNPACK_ALIGNMENT, unpackAlignment );
}
void Platform::OpenGLSetBlendingEnable( bool enable ) {
    if( enable ) {
        glEnable( GL_BLEND );
    } else {
        glDisable( GL_BLEND );
    }
}
bool Platform::OpenGLIsBlendingEnabled() {
    return glIsEnabled( GL_BLEND ) == GL_TRUE ? true : false;
}
void Platform::OpenGLSetBlendFunction( BlendFactor srcColor, BlendFactor dstColor, BlendFactor srcAlpha, BlendFactor dstAlpha ) {
    glBlendFuncSeparate(
        BlendFactorToGLenum( srcColor ),
        BlendFactorToGLenum( dstColor ),
        BlendFactorToGLenum( srcAlpha ),
        BlendFactorToGLenum( dstAlpha )
    );
}
void Platform::OpenGLSetBlendEquation( BlendEq colorEq, BlendEq alphaEq ) {
    glBlendEquationSeparate(
        BlendEqToGLenum( colorEq ),
        BlendEqToGLenum( alphaEq )
    );
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
            LOG_ERROR("OpenGL(%i) > [SRC: %s] [TYPE: %s] %s",
                id,
                DebugSourceToString( source ),
                DebugTypeToString( type ),
                message
            );
        } break;
        case GL_DEBUG_SEVERITY_MEDIUM:
        case GL_DEBUG_SEVERITY_LOW: {
            LOG_WARN("OpenGL(%i) > [SRC: %s] [TYPE: %s] %s",
                id,
                DebugSourceToString( source ),
                DebugTypeToString( type ),
                message
            );
        } break;
        default: case GL_DEBUG_SEVERITY_NOTIFICATION: {
            LOG_INFO("OpenGL(%i) > [SRC: %s] [TYPE: %s] %s",
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
    usize versionLen = stringLen(version) + 1;
    const char* titleAppend = " | OpenGL ";
    usize titleAppendLen = stringLen( titleAppend ) + 1;
    usize newTitleAppendLen = versionLen + titleAppendLen;
    char newTitleAppend[newTitleAppendLen];
    stringConcat(
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
GLenum DataTypeToGLenum( Platform::DataType format ) {
    using namespace Platform;
    switch( format ) {
        case DataType::UNSIGNED_BYTE:  return GL_UNSIGNED_BYTE;
        case DataType::BYTE:           return GL_BYTE;
        case DataType::UNSIGNED_SHORT: return GL_UNSIGNED_SHORT;
        case DataType::SHORT:          return GL_SHORT;
        case DataType::UNSIGNED_INT:   return GL_UNSIGNED_INT;
        case DataType::INT:            return GL_INT;
        case DataType::FLOAT:          return GL_FLOAT;
        case DataType::DOUBLE:         return GL_DOUBLE;
        default: return GL_INVALID_ENUM;
    }
}
GLenum BlendFactorToGLenum( Platform::BlendFactor factor ) {
    using namespace Platform;
    switch( factor ) {
        case BlendFactor::ZERO:                     return GL_ZERO;
        case BlendFactor::ONE:                      return GL_ONE;
        case BlendFactor::SRC_COLOR:                return GL_SRC_COLOR;
        case BlendFactor::ONE_MINUS_SRC_COLOR:      return GL_ONE_MINUS_SRC_COLOR;
        case BlendFactor::DST_COLOR:                return GL_DST_COLOR;
        case BlendFactor::ONE_MINUS_DST_COLOR:      return GL_ONE_MINUS_DST_COLOR;
        case BlendFactor::SRC_ALPHA:                return GL_SRC_ALPHA;
        case BlendFactor::ONE_MINUS_SRC_ALPHA:      return GL_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::DST_ALPHA:                return GL_DST_ALPHA;
        case BlendFactor::ONE_MINUS_DST_ALPHA:      return GL_ONE_MINUS_DST_ALPHA;
        case BlendFactor::CONSTANT_COLOR:           return GL_CONSTANT_COLOR;
        case BlendFactor::ONE_MINUS_CONSTANT_COLOR: return GL_ONE_MINUS_CONSTANT_COLOR;
        case BlendFactor::CONSTANT_ALPHA:           return GL_CONSTANT_ALPHA;
        case BlendFactor::ONE_MINUS_CONSTANT_ALPHA: return GL_ONE_MINUS_CONSTANT_ALPHA;
        case BlendFactor::SRC_ALPHA_SATURATE:       return GL_SRC_ALPHA_SATURATE;
        case BlendFactor::SRC1_COLOR:               return GL_SRC1_COLOR;
        case BlendFactor::ONE_MINUS_SRC1_COLOR:     return GL_ONE_MINUS_SRC1_COLOR;
        case BlendFactor::SRC1_ALPHA:               return GL_SRC1_ALPHA;
        case BlendFactor::ONE_MINUS_SRC1_ALPHA:     return GL_ONE_MINUS_SRC1_ALPHA;
        default: return GL_INVALID_ENUM;
    }
}
GLenum BlendEqToGLenum( Platform::BlendEq eq ) {
    using namespace Platform;
    switch( eq ) {
        case BlendEq::ADD:     return GL_FUNC_ADD;
        case BlendEq::SUB:     return GL_FUNC_SUBTRACT;
        case BlendEq::REV_SUB: return GL_FUNC_REVERSE_SUBTRACT;
        case BlendEq::MIN:     return GL_MIN;
        case BlendEq::MAX:     return GL_MAX;
        default: return GL_INVALID_ENUM;
    }
}

GLenum InternalTextureFormatToGLenum( Platform::TextureFormat format ) {
    using namespace Platform;
    switch( format ) {
        case TextureFormat::R:    return GL_R8;
        case TextureFormat::RG:   return GL_RG8;
        case TextureFormat::RGB:  return GL_RGB8;
        case TextureFormat::RGBA: return GL_RGBA8;
        default: return GL_INVALID_ENUM;
    }
}
