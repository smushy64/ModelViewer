#pragma once

#include "core/text.hpp"
#include "core/ui.hpp"
#include "core/camera.hpp"

#include "consts.hpp"
#include "alias.hpp"
#include "debug.hpp"
#include "backend.hpp"

#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace Platform
{
typedef void (*OpenGLSwapBuffer)(void);
typedef void* (*OpenGLLoader)(const char* functionName);

class UniformBuffer {
public:
    // TODO: usage?

    // Create a new GPU Buffer
    static UniformBuffer* New( usize size, void* data );

public: // NOTE: virtual
    // Buffer Data, size must match total buffer size
    virtual void BufferData( usize size, void* data ) = 0;
    // Buffer Sub Data, offset + size must be less than total buffer size
    virtual void BufferSubData( usize offset, usize size, void* data ) = 0;
    // Set buffer binding point for entire buffer
    virtual void SetBindingPoint( usize point ) = 0;
    // Set buffer binding point for range, offset + size must be less than total buffer size
    virtual void SetBindingPointRange( usize offset, usize size, usize point ) = 0;

    virtual ~UniformBuffer() = default;

public: // NOTE: Getters
    usize Size() const { return m_size; }
    RendererID ID() const { return m_bufferID; }

protected:
    usize      m_size;
    RendererID m_bufferID;
};

enum class BufferDataType {
    FLOAT,
    DOUBLE,
    UBYTE,
    USHORT,
    UINT,
    BYTE,
    SHORT,
    INT,
    BOOL
};

// Get byte size of buffer data type
usize BufferDataTypeByteSize( BufferDataType dataType );
const char* BufferDataTypeToString( BufferDataType dataType );

enum class BufferDataStructure {
    SCALAR,
    VEC2,
    VEC3,
    VEC4,
    MAT2,
    MAT3,
    MAT4
};

// Get number of components of buffer data structure
usize BufferDataStructureCount( BufferDataStructure structure );
const char* BufferDataStructureToString( BufferDataStructure structure );

struct BufferElement {
    bool                normalized;
    BufferDataType      dataType;
    BufferDataStructure dataStructure;
    usize               offset;
    std::string         name;
}; // struct Buffer Element

BufferElement NewBufferElement(
    const std::string&  name,
    BufferDataType      dataType,
    BufferDataStructure dataStructure,
    bool                normalized
);

class BufferLayout {
public:
    BufferLayout( std::vector<BufferElement> elements )
    : m_elements(elements) { CalculateOffsetsAndStride(); }
    BufferLayout() {}

    const std::vector<BufferElement>& Elements() const { return m_elements; }
    usize Stride() const { return m_stride; }
private:
    void CalculateOffsetsAndStride();

    std::vector<BufferElement> m_elements;
    usize m_stride = 0;
}; // class Buffer Layout

class VertexBuffer {
public:
    static VertexBuffer* New( usize size, const void* data );
public: // Getters
    // Get byte size of buffer
    usize Size() const { return m_size; }
    // Get GPU ID
    RendererID ID() const { return m_id; }
    // Get pointer to buffer layout, can be a nullptr so check
    const BufferLayout* GetLayout() const { return &m_bufferLayout; }

public: // NOTE: virtual
    // Bind buffer for use
    virtual void UseBuffer() const = 0;
    // Set the memory layout of this buffer
    virtual void SetLayout( BufferLayout layout ) = 0;

    virtual ~VertexBuffer() = default;
protected:
    usize m_size;
    RendererID m_id;
    BufferLayout m_bufferLayout;
}; // class Vertex Buffer

class IndexBuffer {
public:
    static IndexBuffer* New( BufferDataType type, usize count, const void* data );

public: // Getters
    // Get byte size of buffer
    usize Size() const { return m_size; }
    // Get count of indices
    usize Count() const { return m_count; }
    // Get GPU ID
    RendererID ID() const { return m_id; }
    // Get data type of indices
    BufferDataType DataType() const { return m_type; }

public: // NOTE: virtual
    // Bind buffer for use
    virtual void UseBuffer() const = 0;

    virtual ~IndexBuffer() = default;
protected:
    usize m_size;
    usize m_count;
    RendererID m_id;
    BufferDataType m_type;
}; // class Index Buffer

class VertexArray {
public:
    static VertexArray* New();
public: // Getters
    RendererID ID() const { return m_id; }
    usize VertexBufferCount() const { return m_vertexBuffers.size(); }
    bool HasIndexBuffer() const { return m_indexBuffer != nullptr; }
    const std::vector<VertexBuffer*>& GetVertexBuffers() const { return m_vertexBuffers; }
    const IndexBuffer* GetIndexBuffer() const { return m_indexBuffer; }

public: // NOTE: virtual
    // Bind array for use
    virtual void UseArray() const = 0;
    // Unbind array
    virtual void Unbind() const = 0;
    // Add a vertex buffer to this vertex array, this assumes that the vertex array is in use
    virtual void AddVertexBuffer( VertexBuffer* vertexBuffer ) = 0;
    // Set an index buffer to this vertex array, this assumes that the vertex array is in use
    virtual void SetIndexBuffer( IndexBuffer* indexBuffer ) = 0;

    virtual ~VertexArray() = default;
protected:
    std::vector<VertexBuffer*> m_vertexBuffers;
    IndexBuffer* m_indexBuffer;
    RendererID m_id;
}; // class Vertex Array

class Shader {
public:

    // Create new shader from vertex + fragment source
    static Shader* New( const std::string& vertex, const std::string& fragment );

public: // Getters
    RendererID ID() { return m_id; }
    bool CompilationSucceeded() { return m_success; }

public: // NOTE: virtual
    virtual void UseShader() const = 0;
    virtual bool GetUniform( const std::string& uniformName, UniformID& id ) const = 0;

    virtual void UniformFloat( const UniformID& id, f32 value )               = 0;
    virtual void UniformInt( const UniformID& id, i32 value )                 = 0;
    virtual void UniformVec2( const UniformID& id, const glm::vec2& value )   = 0;
    virtual void UniformVec3( const UniformID& id, const glm::vec3& value )   = 0;
    virtual void UniformVec4( const UniformID& id, const glm::vec4& value )   = 0;
    virtual void UniformMat4( const UniformID& id, const glm::mat4x4& value ) = 0;

protected:
    RendererID m_id;
    bool m_success = false;

public:
    virtual ~Shader() = default;

}; // class Shader

#define AUTO_MIPMAP 0

enum class TextureFormat {
    DEPTH,
    DEPTH_STENCIL,
    R,
    RG,
    RGB,
    RGBA
};
const char* TextureFormatToString( TextureFormat format );

enum class TextureInternalFormat {
    DEPTH,
    DEPTH_STENCIL,
    R,
    RG,
    RGB,
    RGBA
};
const char* TextureInternalFormatToString( TextureInternalFormat format );

enum class TextureWrapMode {
    CLAMP,
    REPEAT,
    MIRROR_REPEAT,
    MIRROR_CLAMP
};
const char* TextureWrapModeToString( TextureWrapMode wrap );

enum class TextureFilterMin {
    NEAREST,
    LINEAR,
    NEAREST_MM_NEAREST,
    LINEAR_MM_NEAREST,
    NEAREST_MM_LINEAR,
    LINEAR_MM_LINEAR
};
const char* TextureFilterMinToString( TextureFilterMin filter );

enum class TextureFilterMag {
    NEAREST,
    LINEAR
};
const char* TextureFilterMagToString( TextureFilterMag filter );

class Texture2D {
public:
    static Texture2D* New(
        const glm::ivec2& dimensions,
        const void* data,
        TextureFormat format,
        TextureInternalFormat internalFormat,
        BufferDataType dataType,
        i32 mipmapLevel,
        bool storeData
    );
    // Don't store data by default
    static Texture2D* New(
        const glm::ivec2& dimensions,
        const void* data,
        TextureFormat format,
        TextureInternalFormat internalFormat,
        BufferDataType dataType,
        i32 mipmapLevel
    ) { return Texture2D::New( dimensions, data, format, internalFormat, dataType, mipmapLevel, false ); }
    // Automatic mipmap levels
    static Texture2D* New(
        const glm::ivec2& dimensions,
        const void* data,
        TextureFormat format,
        TextureInternalFormat internalFormat,
        BufferDataType dataType,
        bool storeData
    ) { return Texture2D::New( dimensions, data, format, internalFormat, dataType, AUTO_MIPMAP, storeData ); }
    // Automatic mipmap levels, don't store data by default
    static Texture2D* New(
        const glm::ivec2& dimensions,
        const void* data,
        TextureFormat format,
        TextureInternalFormat internalFormat,
        BufferDataType dataType
    ) { return Texture2D::New( dimensions, data, format, internalFormat, dataType, AUTO_MIPMAP, false ); }

public: // virtual
    virtual void UseTexture() const = 0;
    // Set Horizontal Wrap Mode, Texture must be bound for this to work properly
    virtual void SetHorizontalWrap( TextureWrapMode wrap ) = 0;
    // Set Vertical Wrap Mode, Texture must be bound for this to work properly
    virtual void SetVerticalWrap( TextureWrapMode wrap ) = 0;
    // Set Minification Filter, Texture must be bound for this to work properly
    virtual void SetMinificationFilter( TextureFilterMin filter ) = 0;
    // Set Magnification Filter, Texture must be bound for this to work properly
    virtual void SetMagnificationFilter( TextureFilterMag filter ) = 0;

    virtual ~Texture2D() = default;

public: // Getters
    RendererID ID() const { return m_id; }
    const glm::ivec2& Dimensions() const { return m_dimensions; }
    const void* Data() const { return m_data; }
    TextureFormat Format() const { return m_format; }
    TextureInternalFormat InternalFormat() const { return m_internalFormat; }
    bool IsStoringData() const { return m_storeData; }
    i32 GetMipmapLevel() const { return m_mipmapLevel; }
    BufferDataType GetDataType() const { return m_dataType; }
    TextureWrapMode GetHorizontalWrapMode() const { return m_horizontalWrap; }
    TextureWrapMode GetVerticalWrapMode() const { return m_verticalWrap; }
    TextureFilterMin GetMinificationFilter() const { return m_minificationFilter; }
    TextureFilterMag GetMagnificationFilter() const { return m_magnificationFilter; }

protected:
    bool                  m_storeData;
    RendererID            m_id;
    TextureFormat         m_format;
    TextureInternalFormat m_internalFormat;
    BufferDataType        m_dataType;
    TextureWrapMode       m_horizontalWrap, m_verticalWrap;
    TextureFilterMin      m_minificationFilter;
    TextureFilterMag      m_magnificationFilter;
    i32                   m_mipmapLevel;
    void*                 m_data = nullptr;
    glm::ivec2            m_dimensions;
    
}; // class Texture2D

const char* BackendToString( BackendAPI backend );

class RendererAPI {
public:

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
    static const char* BlendFactorToString( BlendFactor blendFactor );

    enum class BlendEq {
        ADD,
        SUB,
        REV_SUB,
        MIN,
        MAX
    };
    static const char* BlendEqToString( BlendEq eq );

    enum class PixelAlignment : i32 {
        ONE   = 1,
        TWO   = 2,
        FOUR  = 4,
        EIGHT = 8
    };
    static const char* PixelAlignmentToString( PixelAlignment alignment );

    // Create new renderer API
    static RendererAPI* New( BackendAPI backend );

    // Function for Swapping buffers when using OpenGL
    OpenGLSwapBuffer OpenGLSwapBufferFn;

public: // virtual
    // Initialize API
    virtual void Initialize()  = 0;
    // Clear Buffer
    virtual void ClearBuffer() = 0;
    // Swap Buffers
    virtual void SwapBuffers() = 0;
    // Set Clear Color RGBA 0.0-1.0
    virtual void SetClearColor( const glm::vec4& color )  = 0;
    // Set Viewport Resolution
    virtual void SetViewport( const glm::vec2& viewport ) = 0;
    // Enable wireframe mode
    virtual void EnableWireframe() = 0;
    // Disable wireframe mode
    virtual void DisableWireframe() = 0;
    // Enable blending
    virtual void EnableBlending() = 0;
    // Disable blending
    virtual void DisableBlending() = 0;
    // Set Blend Function
    virtual void BlendFunction(
        BlendFactor srcColor,
        BlendFactor dstColor,
        BlendFactor srcAlpha,
        BlendFactor dstAlpha
    ) = 0;
    // Set Blend Equation
    virtual void BlendEquation( BlendEq Color, BlendEq Alpha ) = 0;
    // Set Constant Blend Color
    virtual void SetConstantBlendColor( const glm::vec4& blendColor ) = 0;
    virtual void SetPackAlignment( PixelAlignment alignment ) = 0;
    virtual void SetUnpackAlignment( PixelAlignment alignment ) = 0;
    // Activate the texture slot at given index
    virtual void SetActiveTexture( u32 activeTexture ) = 0;
    // Draw vertex array, must be bound before use
    virtual void DrawVertexArray( const VertexArray* va ) = 0;

    // Load OpenGL functions, does nothing with other APIs
    virtual bool LoadOpenGLFunctions( OpenGLLoader loader ) = 0;

    virtual ~RendererAPI() = default;

public:
    // Set Clear Color RGB 0.0-1.0
    void SetClearColor( const glm::vec3& color ) { SetClearColor( glm::vec4( color.r, color.g, color.b, 1.0f ) ); }
    // Set Clear Color RGBA 0.0-1.0
    void SetClearColor( f32 r, f32 g, f32 b, f32 a ) { SetClearColor( glm::vec4( r, g, b, a ) ); }
    // Set Clear Color RGB 0.0-1.0
    void SetClearColor( f32 r, f32 g, f32 b ) { SetClearColor( r, g, b, 1.0f ); }
    // Set Viewport Resolution
    void SetViewport( const glm::ivec2& viewport ) { SetViewport( glm::vec2( (f32)viewport.x, (f32)viewport.y ) ); }
    // Set Viewport Resolution
    void SetViewport( f32 width, f32 height ) { SetViewport( glm::vec2( width, height ) ); }
    // Set Blend Function
    void BlendFunction( BlendFactor src, BlendFactor dst ) { BlendFunction( src, dst, src, dst ); }
    // Reset Blend Function to default values
    void ResetBlendFunction() { BlendFunction( BlendFactor::ONE, BlendFactor::ZERO ); }
    // Set Blend Equation
    void BlendEquation( BlendEq eq ) { BlendEquation( eq, eq ); }
    // Set Constant Blend Color
    void SetConstantBlendColor( const glm::vec3& blendColor ) {
        SetConstantBlendColor( glm::vec4( blendColor.r, blendColor.g, blendColor.b, 1.0f ) );
    }
    // Set Constant Blend Color
    void SetConstantBlendColor( f32 r, f32 g, f32 b, f32 a ) { SetConstantBlendColor( r, g, b, a ); }
    // Set Constant Blend Color
    void SetConstantBlendColor( f32 r, f32 g, f32 b ) { SetConstantBlendColor( r, g, b, 1.0f ); }

public: // Getters
    // Get Current Clear Color
    const glm::vec4& GetClearColor() const { return m_clearColor; }
    // Get Viewport Resolution
    const glm::vec2& GetViewport() const { return m_viewport; }
    // Is Wireframe rendering mode enabled
    bool IsWireframeEnabled() const { return m_wireframe; }
    // Is Blending enabled
    bool IsBlendingEnabled() const { return m_blending; }
    // Get Source Color Blend Factor
    BlendFactor SourceColorBlendFactor() const { return m_srcRGB; }
    // Get Source Alpha Blend Factor
    BlendFactor SourceAlphaBlendFactor() const { return m_srcAlpha; }
    // Get Destination Color Blend Factor
    BlendFactor DestColorBlendFactor() const { return m_dstRGB; }
    // Get Destination Alpha Blend Factor
    BlendFactor DestAlphaBlendFactor() const { return m_dstAlpha; }
    // Get Blend Equation for Color
    BlendEq ColorBlendEquation() const { return m_blendEqRGB; }
    // Get Blend Equation for Alpha
    BlendEq AlphaBlendEquation() const { return m_blendEqAlpha; }
    // Get Constant Blend Color
    const glm::vec4& GetConstantBlendColor() const { return m_blendColor; }
    PixelAlignment GetPixelPackAlignment() const { return m_packAlignment; }
    PixelAlignment GetPixelUnpackAlignment() const { return m_unpackAlignment; }
    // Get the index of the active texture slot
    u32 GetActiveTextureIndex() const { return m_activeTexture; }

protected:
    bool m_wireframe = false;
    bool m_blending  = false;

    u32 m_activeTexture = 0;

    BlendFactor m_srcRGB   = BlendFactor::ONE;
    BlendFactor m_dstRGB   = BlendFactor::ZERO;
    BlendFactor m_srcAlpha = BlendFactor::ONE;
    BlendFactor m_dstAlpha = BlendFactor::ZERO;

    BlendEq m_blendEqRGB   = BlendEq::ADD;
    BlendEq m_blendEqAlpha = BlendEq::ADD;

    PixelAlignment m_packAlignment   = PixelAlignment::FOUR;
    PixelAlignment m_unpackAlignment = PixelAlignment::FOUR;

    glm::vec2 m_viewport;
    glm::vec4 m_blendColor;
    glm::vec4 m_clearColor;

}; // class Renderer API

const f32           DEFAULT_TEXT_SCALE = 1.0f;
const glm::vec4     DEFAULT_TEXT_COLOR = glm::vec4(1.0f);
const Core::XAnchor DEFAULT_TEXT_ANCHOR_X = Core::XAnchor::LEFT;
const Core::YAnchor DEFAULT_TEXT_ANCHOR_Y = Core::YAnchor::BOTTOM;

class Renderer {
public:
    Renderer( BackendAPI backend );
    ~Renderer();

    void Initialize();

    // Render Text
    void RenderText( const Core::UI::Label& label );
    void RenderTextButton( const Core::UI::LabelButton& labelButton );
    void RenderText(
        const std::string& text,
        const glm::vec2&   screenSpacePosition,
        f32                scale,
        const glm::vec4&   color,
        Core::XAnchor      anchorX,
        Core::YAnchor      anchorY
    );
    void RenderText(
        const std::string& text,
        const glm::vec2& screenSpacePosition,
        f32 scale,
        const glm::vec4& color
    ) { RenderText( text, screenSpacePosition, scale, color, DEFAULT_TEXT_ANCHOR_X, DEFAULT_TEXT_ANCHOR_Y ); }
    void RenderText(
        const std::string& text,
        const glm::vec2& screenSpacePosition,
        f32 scale
    ) { RenderText( text, screenSpacePosition, scale, DEFAULT_TEXT_COLOR ); }
    void RenderText(
        const std::string& text,
        const glm::vec2& screenSpacePosition
    ) { RenderText( text, screenSpacePosition, DEFAULT_TEXT_SCALE ); }

    void SetCurrentFont( const Core::FontAtlas* font );
    void ResolutionChanged( const glm::vec2& newResolution );
    void UploadFontAtlasBitmap( Core::FontAtlas& fontAtlas );

public: // Getters
    RendererAPI* API() { return m_api; }
    bool Successful() const { return m_success; }

private:
    void RenderCharacter(
        const Core::CharMetrics& metrics,
        const glm::vec2& origin,
        const glm::vec2& pixelPosition,
        f32 scale
    );

private:
    bool m_success;
    RendererAPI* m_api;

    const Core::FontAtlas* m_currentFont = nullptr;

    // 2D ---------------------------------------------

    UniformBuffer* m_matrices2D = nullptr;
    
    Shader* m_fontShader = nullptr;
    UniformID m_fontColorID, m_fontTransformID, m_fontCoordsID;

    VertexArray* m_fontVA;

//     const VertexArray*   GetVertexArray( VertexArrayID id ) { return m_vertexArrays[id]; }

//     Core::Camera& GetCamera() { return m_camera; }

//     // Render a vertex array
//     virtual void RenderVertexArray( const VertexArray* vertexArray ) = 0;

//     // Load font atlas bitmap
//     // Sets font atlas texture id and clears bitmap
//     virtual void LoadFontAtlasBitmap( Core::FontAtlas& fontAtlas ) = 0;

// public:
//     // Render Test Cube
//     void RenderTestCube();
//     // Update View Matrix
//     void UpdateViewMatrix();
//     // Update Projection Matrix
//     void UpdateProjectionMatrix();
//     // Render a vertex array with an ID
//     void RenderVertexArray( VertexArrayID vertexArrayID );
//     // Set clear screen color, RGB 0.0-1.0
//     void SetClearColor( const glm::vec3& clearColor );
//     // Render a label
//     void RenderText( const Core::UI::Label& label );
//     // Render a label button
//     void RenderTextButton( const Core::UI::LabelButton& button );
//     // Push new vertex array
//     VertexArrayID PushVertexArray( VertexArray* vertexArray );

// protected:
//     virtual void RenderCharacter( const Core::CharMetrics& metrics, const glm::vec2& origin ) = 0;
// #ifdef DEBUG
//     virtual void RenderBoundingBox( const glm::vec4& bounds ) = 0;
//     bool m_renderBoundingBoxes = false;
// #endif

//     // 3D ---------------------------------------------

//     Core::Camera m_camera = Core::Camera();

//     UniformBuffer* m_matrices3D = nullptr;
//     Shader*   m_modelShader = nullptr;
//     UniformID m_modelTransformID;

}; // class Renderer

} // namespace Platform

