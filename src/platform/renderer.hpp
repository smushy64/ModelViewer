/**
 * Description:  Renderer
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 15, 2022 
*/
#pragma once
#include "pch.hpp"

namespace Platform {

struct Shader {
    u32 id;
};

enum class Structure {
    SCALAR,
    VEC2,
    VEC3,
    VEC4,
    MAT3,
    MAT4
};
const char* StructureToString( Structure structure );

enum class Type {
    UNSIGNED_BYTE,
    BYTE,
    UNSIGNED_SHORT,
    SHORT,
    UNSIGNED_INT,
    INT,
    FLOAT,
    DOUBLE
};
const char* TypeToString( Type type );

enum class TextureFormat {
    R, RG, RGB, RGBA
};
const char* TextureFormatToString( TextureFormat format );
enum class TextureWrapMode {
    CLAMP,
    REPEAT,
    MIRROR_REPEAT,
    MIRROR_CLAMP
};
const char* TextureWrapModeToString( TextureWrapMode wrapMode );
enum class TextureMagFilter {
    NEAREST,
    LINEAR
};
const char* TextureMagFilterToString( TextureMagFilter magFilter );
enum class TextureMinFilter {
    NEAREST,
    LINEAR,
    LINEAR_MIPMAP_NEAREST,
    LINEAR_MIPMAP_LINEAR,
    NEAREST_MIPMAP_NEAREST,
    NEAREST_MIPMAP_LINEAR
};
const char* TextureMinFilterToString( TextureMinFilter minFilter );

struct Texture2D {
    i32 width;
    i32 height;
    Type             dataType;
    TextureFormat    format;
    TextureWrapMode  wrapModeX;
    TextureWrapMode  wrapModeY;
    TextureMagFilter magFilter;
    TextureMinFilter minFilter;
    u8* data;
    u32 id;

    const char* toString();
};

enum class RendererBackend {
    OPENGL
};
/// @brief Convert renderer backend enum to string
const char* RendererBackendToString( RendererBackend backend );

typedef void (*InitializeFN)();
typedef void (*ClearBufferFN)();
typedef void (*SwapBuffersFN)();
typedef void (*SetClearColorFN)( f32 r, f32 g, f32 b, f32 a );
typedef void (*SetViewportFN)( i32 width, i32 height );

// NOTE(alicia): Shader
typedef bool (*CreateShaderFN)( const char* vertexSrc, usize vertexLen, const char* fragmentSrc, usize fragmentLen, Shader* result );
typedef void (*DeleteShadersFN)( Shader* shaders, usize shaderCount );
typedef void (*UseShaderFN)( Shader* shader );
typedef bool (*GetUniformIDFN)( Shader* shader, const char* uniformName, i32* result );
typedef void (*UniformFloatFN)( Shader* shader, i32 uniform, f32 value );
typedef void (*UniformUIntFN)( Shader* shader, i32 uniform, u32 value );
typedef void (*UniformIntFN)( Shader* shader, i32 uniform, i32 value );

// NOTE(alicia): Texture2D
typedef Texture2D (*CreateTexture2DFN)(
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
typedef void (*DeleteTextures2DFN)( Texture2D* textures, usize textureCount );
typedef void (*UseTexture2DFN)( Texture2D* texture, u32 unit );
typedef void (*SetTexture2DWrapModeFN)( Texture2D* texture, TextureWrapMode wrapX, TextureWrapMode wrapY );
typedef void (*SetTexture2DFilterFN)( Texture2D* texture, TextureMinFilter minFilter, TextureMagFilter magFilter );

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
    /// @param r,g,b,a [f32] Clear color
    SetClearColorFN SetClearColor;
    /// @brief Set viewport dimensions
    /// @param width [i32] Width in pixels
    /// @param height [i32] Height in pixels
    SetViewportFN SetViewport;

    // NOTE(alicia): Buffers

    // NOTE(alicia): Shaders

    /// @brief Create Shader from source
    /// @param vertexSrc [const char*] vertex shader source
    /// @param vertexLen [usize] vertex source length
    /// @param fragmentSrc [const char*] fragment shader source
    /// @param fragmentLen [usize] fragment source length
    /// @param result [Shader*] pointer to shader structure to hold result
    /// @return true if successful, false if not
    CreateShaderFN CreateShader;
    /// @brief Delete shaders
    /// @param shaders [Shader*] shaders to delete
    /// @param shaderCount [usize] number of shaders to delete
    DeleteShadersFN DeleteShaders;
    /// @brief Use given shader
    /// @param shader [Shader*] shader to use
    UseShaderFN UseShader;
    /// @brief Get the ID of a given uniform
    /// @param shader [Shader*] shader to query
    /// @param uniformName [const char*] name of uniform
    /// @param result [i32*] pointer to i32 to hold result
    /// @return true if uniform found
    GetUniformIDFN GetUniformID;
    /// @brief Upload f32 to Shader Uniform
    /// @param shader [Shader*] shader
    /// @param uniform [i32] uniform ID
    /// @param value [f32] value
    UniformFloatFN UniformFloat;
    /// @brief Upload u32 to Shader Uniform
    /// @param shader [Shader*] shader
    /// @param uniform [i32] uniform ID
    /// @param value [u32] value
    UniformUIntFN UniformUInt;
    /// @brief Upload i32 to Shader Uniform
    /// @param shader [Shader*] shader
    /// @param uniform [i32] uniform ID
    /// @param value [i32] value
    UniformIntFN UniformInt;

    // NOTE(alicia): Texture 2D

    /// @brief Create texture 2D
    /// @param width [i32] texture width
    /// @param height [i32] texture height
    /// @param data [u8*] texture data
    /// @param format [TextureFormat] data format
    /// @param dataType [Type] data type
    /// @param wrapX [TextureWrapMode] wrap mode on x-axis
    /// @param wrapY [TextureWrapMode] wrap mode on y-axis
    /// @param minFilter [TextureMinFilter] minification filtering
    /// @param magFilter [TextureMinFilter] magnification filtering
    /// @return [Texture2D] texture 2D
    CreateTexture2DFN CreateTexture2D;
    /// @brief Delete textures
    /// @param textures [Texture2D*] textures to delete
    /// @param textureCount [usize] number of textures to delete
    DeleteTextures2DFN DeleteTextures2D;
    /// @brief Use given texture
    /// @param texture [Texture2D*] texture to use
    /// @param unit [u32] which texture unit to use texture in
    UseTexture2DFN UseTexture2D;
    /// @brief Set texture wrapping mode
    /// @param texture [Texture2D*] texture to modify
    /// @param wrapX [TextureWrapMode] wrap mode on x-axis
    /// @param wrapY [TextureWrapMode] wrap mode on y-axis
    SetTexture2DWrapModeFN SetTexture2DWrapMode;
    /// @brief Set texture filtering
    /// @param texture [Texture2D*] texture to modify
    /// @param minFilter [TextureMinFilter] minification filtering
    /// @param magFilter [TextureMagFilter] magnification filtering
    SetTexture2DFilterFN SetTexture2DFilter;
};

typedef void* (*OpenGLLoadProc)(const char* functionName);

/// @brief Create OpenGL API
/// @param api pointer to structure to hold functions
/// @param loadProc OpenGL load proc
/// @return true if successful
bool CreateOpenGLAPI( RendererAPI* api, OpenGLLoadProc loadProc );

} // namespace Platform

