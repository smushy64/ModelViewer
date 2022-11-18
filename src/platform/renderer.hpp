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

enum class BlendFactor {
    ZERO,
    ONE,
    SRC_COLOR,
    ONE_MINUS_SRC_COLOR,
    DST_COLOR,
    ONE_MINUS_DST_COLOR,
    SRC_ALPHA,
    ONE_MINUS_SRC_ALPHA,
    DST_ALPHA,
    ONE_MINUS_DST_ALPHA,
    CONSTANT_COLOR,
    ONE_MINUS_CONSTANT_COLOR,
    CONSTANT_ALPHA,
    ONE_MINUS_CONSTANT_ALPHA,
    SRC_ALPHA_SATURATE,
    SRC1_COLOR,
    ONE_MINUS_SRC1_COLOR,
    SRC1_ALPHA,
    ONE_MINUS_SRC1_ALPHA
};
const char* BlendFactorToString( BlendFactor blendFactor );

enum class BlendEq {
    ADD,
    SUB,
    REV_SUB,
    MIN,
    MAX
};
const char* BlendEqToString( BlendEq eq );

enum class DataStructure {
    SCALAR,
    VEC2,
    VEC3,
    VEC4,
    MAT3,
    MAT4
};
const char* DataStructureToString( DataStructure structure );
usize DataStructureCount( DataStructure structure );

enum class DataType {
    UNSIGNED_BYTE,
    BYTE,
    UNSIGNED_SHORT,
    SHORT,
    UNSIGNED_INT,
    INT,
    FLOAT,
    DOUBLE
};
const char* DataTypeToString( DataType type );
usize DataTypeSize( DataType type );

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
    DataType         dataType;
    TextureFormat    format;
    TextureWrapMode  wrapModeX;
    TextureWrapMode  wrapModeY;
    TextureMagFilter magFilter;
    TextureMinFilter minFilter;
    u8* data;
    u32 id;
};

struct UniformBuffer {
    usize size;
    u32 id;
};

struct Vertex {
    smath::vec3 position;
    smath::vec2 uv;
    smath::vec3 normal;
    smath::vec3 tangent;
    smath::vec4 bitangent;
};

struct IndexBuffer {
    usize    bufferSize;
    usize    indexCount;
    void*    indices;
    DataType dataType;
    u32      id;
};

struct VertexBufferElement {
    DataStructure structure;
    DataType      dataType;
    bool          normalized;
};

struct VertexBufferLayout {
    usize                elementCount;
    VertexBufferElement* elements;
    usize*               elementOffsets;
    usize                stride;
};
VertexBufferLayout CreateVertexBufferLayout( usize elementCount, VertexBufferElement* elements );
void FreeVertexBufferLayout( VertexBufferLayout* layout );

struct VertexBuffer {
    usize   bufferSize;
    void*   vertices;
    u32     id;
    VertexBufferLayout layout;
};

struct VertexArray {
    usize vertexBufferCount;
    VertexBuffer* buffers;
    IndexBuffer* indexBuffer;

    u32 id;
};

enum class RendererBackend {
    OPENGL
};
/// @brief Convert renderer backend enum to string
const char* RendererBackendToString( RendererBackend backend );

#define RENDERER_PACK_ALIGNMENT_1 1
#define RENDERER_PACK_ALIGNMENT_4 4
#define RENDERER_PACK_ALIGNMENT_8 8
#define RENDERER_PACK_ALIGNMENT_DEFAULT RENDERER_PACK_ALIGNMENT_4

typedef void (*InitializeFN)();
typedef void (*ClearBufferFN)();
typedef void (*SwapBuffersFN)();
typedef void (*SetClearColorFN)( f32 r, f32 g, f32 b, f32 a );
typedef void (*SetViewportFN)( i32 width, i32 height );
typedef void (*SetPackAlignmentFN)( i32 packAlignment );
typedef void (*SetUnPackAlignmentFN)( i32 unpackAlignment );
typedef void (*SetBlendingEnableFN)( bool enable );
typedef bool (*IsBlendingEnabledFN)();
typedef void (*SetBlendFunctionFN)( BlendFactor srcColor, BlendFactor dstColor, BlendFactor srcAlpha, BlendFactor dstAlpha );
typedef void (*SetBlendEquationFN)( BlendEq colorEq, BlendEq alphaEq );

// NOTE(alicia): Vertex Array
typedef VertexArray (*CreateVertexArrayFN)();
typedef void (*DeleteVertexArraysFN)( usize count, VertexArray* vertexArrays );
typedef void (*UseVertexArrayFN)( VertexArray* vertexArray );
typedef void (*VertexArrayBindVertexBufferFN)( VertexArray* vertexArray, VertexBuffer buffer );
typedef void (*VertexArrayBindIndexBufferFN)( VertexArray* vertexArray, IndexBuffer buffer );

// NOTE(alicia): Vertex Buffer
typedef VertexBuffer (*CreateVertexBufferFN)( usize bufferSize, void* vertices, VertexBufferLayout layout );
typedef void (*UseVertexBufferFN)( VertexBuffer* buffer );
typedef void (*DeleteVertexBuffersFN)( usize count, VertexBuffer* buffers );

// NOTE(alicia): Index Buffer
typedef IndexBuffer (*CreateIndexBufferFN)( usize bufferSize, void* indices, DataType indexDataType );
typedef void (*UseIndexBufferFN)( IndexBuffer* buffer );
typedef void (*DeleteIndexBuffersFN)( usize count, IndexBuffer* buffers );

// NOTE(alicia): Uniform buffer
typedef UniformBuffer (*CreateUniformBufferFN)( usize size, void* data );
typedef void (*DeleteUniformBuffersFN)( UniformBuffer* buffers, usize bufferCount );
typedef void (*UniformBufferDataFN)(UniformBuffer* uniformBuffer, usize size, void* data);
typedef void (*UniformBufferSubDataFN)(UniformBuffer* uniformBuffer, usize offset, usize size, void* data);
typedef void (*UniformBufferSetBindingPointFN)(UniformBuffer* uniformBuffer, u32 bindingPoint);
typedef void (*UniformBufferSetBindingPointRangeFN)(UniformBuffer* uniformBuffer, u32 bindingPoint, usize offset, usize size);

// NOTE(alicia): Buffers
typedef void (*DeleteBuffersFN)( usize bufferCount, u32* bufferIDs );

// NOTE(alicia): Shader
typedef bool (*CreateShaderFN)( const char* vertexSrc, usize vertexLen, const char* fragmentSrc, usize fragmentLen, Shader* result );
typedef void (*DeleteShadersFN)( Shader* shaders, usize shaderCount );
typedef void (*UseShaderFN)( Shader* shader );
typedef bool (*GetUniformIDFN)( Shader* shader, const char* uniformName, i32* result );
typedef void (*UniformFloatFN)( Shader* shader, i32 uniform, f32 value );
typedef void (*UniformUIntFN)( Shader* shader, i32 uniform, u32 value );
typedef void (*UniformIntFN)( Shader* shader, i32 uniform, i32 value );
typedef void (*UniformVec2FN)( Shader* shader, i32 uniform, smath::vec2* value );
typedef void (*UniformVec3FN)( Shader* shader, i32 uniform, smath::vec3* value );
typedef void (*UniformVec4FN)( Shader* shader, i32 uniform, smath::vec4* value );
typedef void (*UniformMat3FN)( Shader* shader, i32 uniform, smath::mat3* value );
typedef void (*UniformMat4FN)( Shader* shader, i32 uniform, smath::mat4* value );

// NOTE(alicia): Texture2D
typedef Texture2D (*CreateTexture2DFN)(
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
    /// @brief Set pack alignment
    /// @param packAlignment [i32]
    SetPackAlignmentFN SetPackAlignment;
    /// @brief Set unpack alignment
    /// @param unpackAlignment [i32]
    SetUnPackAlignmentFN SetUnPackAlignment;
    /// @brief Set blending enabled or disabled
    SetBlendingEnableFN SetBlendingEnable;
    /// @brief Check if blending is enabled
    IsBlendingEnabledFN IsBlendingEnabled;
    /// @brief Set blending function
    /// @param srcColor source color factor
    /// @param dstColor destination color factor
    /// @param srcAlpha source alpha factor
    /// @param dstAlpha destination alpha factor
    SetBlendFunctionFN SetBlendFunction;
    /// @brief Set blending equation
    /// @param colorEq color equation
    /// @param alphaEq alpha equation
    SetBlendEquationFN SetBlendEquation;

    /// @brief Create new vertex array 
    CreateVertexArrayFN CreateVertexArray;
    /// @brief Delete vertex arrays
    /// @param count [usize] number of vertex arrays to delete
    /// @param vertexArrays [VertexArray*] vertex arrays
    DeleteVertexArraysFN DeleteVertexArrays;
    /// @brief Use a vertex array for rendering
    UseVertexArrayFN UseVertexArray;
    /// @brief Bind vertex buffer to vertex array
    /// @param vertexArray [VertexArray*] vertex array
    /// @param buffer [VertexBuffer*] buffer to bind
    VertexArrayBindVertexBufferFN VertexArrayBindVertexBuffer;
    /// @brief Bind index buffer to vertex array
    /// @param vertexArray [VertexArray*] vertex array
    /// @param buffer [IndexBuffer*] buffer to bind
    VertexArrayBindIndexBufferFN VertexArrayBindIndexBuffer;
    /// @brief Create new vertex buffer
    /// @param bufferSize [usize] size of buffer in bytes
    /// @param vertices [void*] vertices
    /// @param layout [VertexBufferLayout] memory layout of vertices
    CreateVertexBufferFN CreateVertexBuffer;
    /// @brief Use a vertex buffer for rendering
    UseVertexBufferFN UseVertexBuffer;
    /// @brief Delete vertex buffers
    /// @param count [usize] number of vertex buffers to delete
    /// @param vertexArrays [VertexBuffer*] vertex buffers
    DeleteVertexBuffersFN DeleteVertexBuffers;
    /// @brief Create new index buffer
    /// @param bufferSize [usize] size of buffer in bytes
    /// @param indices [void*] indices
    /// @param indexDataType [DataType] data type of indices
    CreateIndexBufferFN CreateIndexBuffer;
    /// @brief Use an index buffer for rendering
    UseIndexBufferFN UseIndexBuffer;
    /// @brief Delete index buffers
    /// @param count [usize] number of index buffers to delete
    /// @param buffers [IndexBuffer*] index buffers
    DeleteIndexBuffersFN DeleteIndexBuffers;

    // NOTE(alicia): Uniform Buffer

    /// @brief Create uniform buffer
    /// @param size [usize] size of buffer
    /// @param data [void*] data
    CreateUniformBufferFN CreateUniformBuffer;
    /// @brief Delete uniform buffers
    /// @param buffers [UniformBuffer*] buffers to delete
    /// @param bufferCount [usize] number of buffers
    DeleteUniformBuffersFN DeleteUniformBuffers;
    /// @brief Upload data to uniform buffer
    /// @param uniformBuffer [UniformBuffer*] Uniform buffer to buffer data into
    /// @param size [usize] size of data
    /// @param data [void*] data
    /// @exception if size != uniform buffer size, this function will fail
    UniformBufferDataFN UniformBufferData;
    /// @brief Upload data to uniform buffer
    /// @param uniformBuffer [UniformBuffer*] Uniform buffer to buffer data into
    /// @param offset [usize] offset where data should be placed
    /// @param size [usize] size of data
    /// @param data [void*] data
    /// @exception if offset + size > uniform buffer size, this function will fail
    UniformBufferSubDataFN UniformBufferSubData;
    /// @brief Bind entire buffer to a given point
    /// @param uniformBuffer [UniformBuffer*] Uniform buffer to bind
    /// @param bindingPoint [u32] point to bind buffer to
    UniformBufferSetBindingPointFN UniformBufferSetBindingPoint;
    /// @brief Bind buffer range to a given point
    /// @param uniformBuffer [UniformBuffer*] Uniform buffer to bind
    /// @param bindingPoint [u32] point to bind buffer to
    /// @param offset [usize] byte where binding should begin
    /// @param size [usize] size of range
    UniformBufferSetBindingPointRangeFN UniformBufferSetBindingPointRange;

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
    /// @brief Upload vec2 to Shader Uniform
    /// @param shader [Shader*] shader
    /// @param uniform [i32] uniform ID
    /// @param value [vec2*] value
    UniformVec2FN UniformVec2;
    /// @brief Upload vec3 to Shader Uniform
    /// @param shader [Shader*] shader
    /// @param uniform [i32] uniform ID
    /// @param value [vec3*] value
    UniformVec3FN UniformVec3;
    /// @brief Upload vec4 to Shader Uniform
    /// @param shader [Shader*] shader
    /// @param uniform [i32] uniform ID
    /// @param value [vec4*] value
    UniformVec4FN UniformVec4;
    /// @brief Upload mat3 to Shader Uniform
    /// @param shader [Shader*] shader
    /// @param uniform [i32] uniform ID
    /// @param value [mat3*] value
    UniformMat3FN UniformMat3;
    /// @brief Upload mat4 to Shader Uniform
    /// @param shader [Shader*] shader
    /// @param uniform [i32] uniform ID
    /// @param value [mat4*] value
    UniformMat4FN UniformMat4;

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

