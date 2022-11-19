/**
 * Description:  OpenGL
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 15, 2022 
*/
#pragma once
#include "pch.hpp"

namespace Platform {

typedef void* (*OpenGLLoadProc)(const char* functionName);
struct Shader;
struct Texture2D;
struct UniformBuffer;
struct VertexArray;
struct VertexBuffer;
struct IndexBuffer;
struct VertexBufferLayout;
enum class TextureFormat;
enum class DataType;
enum class TextureWrapMode;
enum class TextureMinFilter;
enum class TextureMagFilter;
enum class BlendFactor;
enum class BlendEq;

bool OpenGLLoadFunctions( OpenGLLoadProc loadProc );
void OpenGLInitialize();
void OpenGLClearBuffer();
void OpenGLSetClearColor( f32 r, f32 g, f32 b, f32 a );
void OpenGLSetViewport( i32 width, i32 height );
void OpenGLSetPackAlignment( i32 packAlignment );
void OpenGLSetUnPackAlignment( i32 unpackAlignment );
void OpenGLSetBlendingEnable( bool enable );
bool OpenGLIsBlendingEnabled();
void OpenGLSetBlendFunction( BlendFactor srcColor, BlendFactor dstColor, BlendFactor srcAlpha, BlendFactor dstAlpha );
void OpenGLSetBlendEquation( BlendEq colorEq, BlendEq alphaEq );
void OpenGLDrawVertexArray( VertexArray* vertexArray );

// NOTE(alicia): shader

bool OpenGLCreateShader(
    const char* vertexSrc,
    usize vertexLen,
    const char* fragmentSrc,
    usize fragmentLen,
    Shader* result
);
void OpenGLDeleteShaders( usize shaderCount, Shader* shaders );
void OpenGLUseShader( Shader* shader );
bool OpenGLGetUniformID( Shader* shader, const char* uniformName, i32* result );
void OpenGLUniformFloat( Shader* shader, i32 uniform, f32 value );
void OpenGLUniformUInt( Shader* shader, i32 uniform, u32 value );
void OpenGLUniformInt( Shader* shader, i32 uniform, i32 value );
void OpenGLUniformVec2( Shader* shader, i32 uniform, smath::vec2* value );
void OpenGLUniformVec3( Shader* shader, i32 uniform, smath::vec3* value );
void OpenGLUniformVec4( Shader* shader, i32 uniform, smath::vec4* value );
void OpenGLUniformMat3( Shader* shader, i32 uniform, smath::mat3* value );
void OpenGLUniformMat4( Shader* shader, i32 uniform, smath::mat4* value );

// NOTE(alicia): texture 2D

Texture2D OpenGLCreateTexture2D(
    i32 width,
    i32 height,
    u8* data,
    TextureFormat format,
    DataType dataType,
    TextureWrapMode wrapX,
    TextureWrapMode wrapY,
    TextureMinFilter minFilter,
    TextureMagFilter magFilter
);
void OpenGLDeleteTextures2D( usize textureCount, Texture2D* textures );
void OpenGLUseTexture2D( Texture2D* texture, u32 unit );
void OpenGLSetTexture2DWrapMode( Texture2D* texture, TextureWrapMode wrapX, TextureWrapMode wrapY );
void OpenGLSetTexture2DFilter( Texture2D* texture, TextureMinFilter minFilter, TextureMagFilter magFilter );

// NOTE(alicia): Vertex Array

VertexArray OpenGLCreateVertexArray();
void OpenGLDeleteVertexArrays( usize count, VertexArray* vertexArrays );
void OpenGLUseVertexArray( VertexArray* vertexArray );
void OpenGLVertexArrayBindVertexBuffer( VertexArray* vertexArray, VertexBuffer buffer );
void OpenGLVertexArrayBindIndexBuffer( VertexArray* vertexArray, IndexBuffer buffer );

// NOTE(alicia): Vertex Buffer

VertexBuffer OpenGLCreateVertexBuffer( usize bufferSize, void* vertices, VertexBufferLayout layout );
void OpenGLUseVertexBuffer( VertexBuffer* buffer );
void OpenGLDeleteVertexBuffers( usize count, VertexBuffer* buffers );

// NOTE(alicia): Index Buffer

IndexBuffer OpenGLCreateIndexBuffer( usize indexCount, void* indices, DataType indexDataType );
void OpenGLUseIndexBuffer( IndexBuffer* buffer );
void OpenGLDeleteIndexBuffers( usize count, IndexBuffer* buffers );

// NOTE(alicia): Uniform Buffer

UniformBuffer OpenGLCreateUniformBuffer( usize size, void* data );
void OpenGLDeleteUniformBuffers( usize bufferCount, UniformBuffer* buffers );
void OpenGLUniformBufferData(UniformBuffer* uniformBuffer, usize size, void* data);
void OpenGLUniformBufferSubData(UniformBuffer* uniformBuffer, usize offset, usize size, void* data);
void OpenGLUniformBufferSetBindingPoint(UniformBuffer* uniformBuffer, u32 bindingPoint);
void OpenGLUniformBufferSetBindingPointRange(UniformBuffer* uniformBuffer, u32 bindingPoint, usize offset, usize size);

// NOTE(alicia): Buffers
void OpenGLDeleteBuffers( usize bufferCount, u32* bufferIDs );

} // namespace Platform
