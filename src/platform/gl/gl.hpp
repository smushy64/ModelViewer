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
enum class TextureFormat;
enum class Type;
enum class TextureWrapMode;
enum class TextureMinFilter;
enum class TextureMagFilter;

bool OpenGLLoadFunctions( OpenGLLoadProc loadProc );
void OpenGLInitialize();
void OpenGLClearBuffer();
void OpenGLSetClearColor( f32 r, f32 g, f32 b, f32 a );
void OpenGLSetViewport( i32 width, i32 height );

// NOTE(alicia): shader

bool OpenGLCreateShader(
    const char* vertexSrc,
    usize vertexLen,
    const char* fragmentSrc,
    usize fragmentLen,
    Shader* result
);
void OpenGLDeleteShaders( Shader* shaders, usize shaderCount );
void OpenGLUseShader( Shader* shader );
bool OpenGLGetUniformID( Shader* shader, const char* uniformName, i32* result );
void OpenGLUniformFloat( Shader* shader, i32 uniform, f32 value );
void OpenGLUniformUInt( Shader* shader, i32 uniform, u32 value );
void OpenGLUniformInt( Shader* shader, i32 uniform, i32 value );

// NOTE(alicia): texture 2D
Texture2D OpenGLCreateTexture2D(
    i32 width,
    i32 height,
    u8* data,
    TextureFormat format,
    Type dataType,
    TextureWrapMode wrapX,
    TextureWrapMode wrapY,
    TextureMinFilter minFilter,
    TextureMagFilter magFilter
);
void OpenGLDeleteTextures2D( Texture2D* textures, usize textureCount );
void OpenGLUseTexture2D( Texture2D* texture, u32 unit );
void OpenGLSetTexture2DWrapMode( Texture2D* texture, TextureWrapMode wrapX, TextureWrapMode wrapY );
void OpenGLSetTexture2DFilter( Texture2D* texture, TextureMinFilter minFilter, TextureMagFilter magFilter );

} // namespace Platform


