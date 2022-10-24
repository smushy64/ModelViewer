#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include "core/text.hpp"
#include "core/ui.hpp"
#include "consts.hpp"
#include "alias.hpp"
#include "debug.hpp"

#include <string>
#include <vector>

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

struct BufferElement {
    bool                normalized;
    BufferDataType      dataType;
    BufferDataStructure dataStructure;
    usize               size;
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
    virtual void UseBuffer() = 0;
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
    virtual void UseBuffer() = 0;

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
    virtual void UseArray() = 0;
    // Unbind array
    virtual void Unbind() = 0;
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
    virtual void UseShader() = 0;
    virtual bool GetUniform( const std::string& uniformName, UniformID& id ) = 0;

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

class Renderer {
public:

    enum class Backend {
        OPENGL
    };

    // Create new renderer with given Backend
    static Renderer* New( Backend backend );

    // Getters

    const glm::vec4& GetClearColor()   { return m_clearColor; }
    const glm::vec2& GetViewport()     { return m_viewport;   }

    const glm::vec2&     GetTextPixelPosition()  { return m_textPosition; }
    const glm::vec2&     GetTextScreenPosition() { return m_textScreenPosition; }
    const glm::vec4&     GetTextColor()    { return m_textColor; }
    const f32&           GetTextScale()    { return m_textScale; }
    const Core::XAnchor& GetTextXAnchor()  { return m_textXAnchor; }
    const Core::YAnchor& GetTextYAnchor()  { return m_textYAnchor; }

public: // NOTE: virtual
    virtual bool Initialize()   = 0;
    // Clear Screen buffer
    virtual void ClearBuffer()  = 0;
    // Swap Screen buffers
    virtual void SwapBuffer()   = 0;
    // Set clear screen color, RGBA 0.0-1.0
    virtual void SetClearColor( const glm::vec4& clearColor ) = 0;
    // Set viewport scale
    virtual void SetViewport( const glm::vec2& viewport ) = 0;

    // Set text color, RGBA 0.0-1.0
    virtual void SetTextColor( const glm::vec4& color ) = 0;
    // Render text with internal settings
    // Doesn't do anything if font atlas pointer is null
    virtual void RenderText( const std::string& text ) = 0;
    // Load font atlas bitmap
    // Sets font atlas texture id and clears bitmap
    virtual void LoadFontAtlasBitmap( Core::FontAtlas& fontAtlas ) = 0;

    // Load OpenGL functions, does nothing with other APIs
    virtual bool LoadOpenGLFunctions( OpenGLLoader loader ) = 0;
public:
    // Set clear screen color, RGB 0.0-1.0
    void SetClearColor( const glm::vec3& clearColor );
    // Render a label
    void RenderText( const Core::UI::Label& label );
    // Render a label button
    void RenderTextButton( const Core::UI::LabelButton& button );

    // Sets internal font atlas pointer to this font atlas
    void UseFontAtlas( const Core::FontAtlas& fontAtlas );
    // Sets internal font atlas pointer to this font atlas
    void UseFontAtlas( const Core::FontAtlas* fontAtlas );
    // Set text color, RGB 0.0-1.0
    void SetTextColor( const glm::vec3& color );
    // Set text anchor on x axis
    void SetTextXAnchor( const Core::XAnchor& anchor );
    // Set text anchor on y axis
    void SetTextYAnchor( const Core::YAnchor& anchor );
    // Set text anchor on both axis
    void SetTextAnchors( const Core::XAnchor& xAnchor, const Core::YAnchor& yAnchor );
    // Set text scale in screen space
    void SetTextScale( f32 scale );
    // Set text position in screen space
    void SetTextPosition( const glm::vec2& screenSpacePosition );
    // Reset all text parameters
    void ResetTextParameters();
    // Set Viewport scale
    void SetViewport( const glm::ivec2& viewport );

    // Enable rendering bounding boxes
    void EnableBoundingBox() {
#ifdef DEBUG
        LOG_DEBUG("Renderer > Bounding Box enabled.");
        m_renderBoundingBoxes = true;
#endif
    }
    // Disable rendering bounding boxes
    void DisableBoundingBox() {
#ifdef DEBUG
        LOG_DEBUG("Renderer > Bounding Box disabled.");
        m_renderBoundingBoxes = false;
#endif
    }

protected:
    virtual void RenderCharacter( const Core::CharMetrics& metrics, const glm::vec2& origin ) = 0;
#ifdef DEBUG
    virtual void RenderBoundingBox( const glm::vec4& bounds ) = 0;
    bool m_renderBoundingBoxes = false;
#endif

    glm::vec4 m_clearColor = glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f );
    glm::vec2 m_viewport   = glm::vec2( DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT );

    const Core::FontAtlas* m_fontAtlas = nullptr;
    glm::vec4 m_textColor        = Core::UI::DEFAULT_TEXT_COLOR;
    Core::XAnchor m_textXAnchor  = Core::UI::DEFAULT_TEXT_X_ANCHOR;
    Core::YAnchor m_textYAnchor  = Core::UI::DEFAULT_TEXT_Y_ANCHOR;
    const f32 DEFAULT_TEXT_SCALE = 1.0f;
    f32 m_textScale = DEFAULT_TEXT_SCALE;

    glm::vec2 m_textPosition       = Core::UI::DEFAULT_TEXT_POSITION;
    glm::vec2 m_textScreenPosition = Core::UI::DEFAULT_TEXT_POSITION;

    Shader* m_fontShader = nullptr;
    UniformID m_fontColorID, m_fontTransformID, m_fontCoordsID;

    Shader* m_boundsShader = nullptr;
    UniformID m_boundsTransformID;

    UniformBuffer* m_matrices2D = nullptr;

    VertexArray* m_fontMesh = nullptr;
public:
    OpenGLSwapBuffer OpenGLSwapBufferFn;

    virtual ~Renderer() = default;
}; // class Renderer

} // namespace Platform

