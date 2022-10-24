#pragma once
#include "renderer.hpp"

namespace Platform {

u32 BufferDataTypeToGLenum( BufferDataType dataType );

class RendererOpenGL : public Renderer {
public: // NOTE: virtual
    virtual ~RendererOpenGL() override;
    virtual bool Initialize()  override;
    virtual void ClearBuffer() override;
    virtual void SwapBuffer()  override;
    virtual void SetClearColor( const glm::vec4& clearColor ) override;
    virtual void SetViewport( const glm::vec2& viewport ) override;

    virtual void SetTextColor( const glm::vec4& color ) override;
    virtual void LoadFontAtlasBitmap( Core::FontAtlas& fontAtlas ) override;
    virtual void RenderText( const std::string& text ) override;

    virtual bool LoadOpenGLFunctions( OpenGLLoader loader ) override;

protected:
    virtual void RenderCharacter( const Core::CharMetrics& metrics, const glm::vec2& origin ) override;
    virtual void RenderBoundingBox( const glm::vec4& bounds ) override;
private:
    RendererID m_fontVAO, m_fontVBO, m_fontEBO;
    RendererID m_boundsVAO, m_boundsVBO, m_boundsEBO;
}; // class Renderer

class ShaderOpenGL : public Shader {
public:
    ShaderOpenGL( const std::string& vertex, const std::string& fragment );

public: // NOTE: virtual
    virtual void UseShader() override;
    virtual bool GetUniform( const std::string& uniformName, UniformID& id )  override;

    virtual void UniformFloat( const UniformID& id, f32 value )               override;
    virtual void UniformInt( const UniformID& id, i32 value )                 override;
    virtual void UniformVec2( const UniformID& id, const glm::vec2& value )   override;
    virtual void UniformVec3( const UniformID& id, const glm::vec3& value )   override;
    virtual void UniformVec4( const UniformID& id, const glm::vec4& value )   override;
    virtual void UniformMat4( const UniformID& id, const glm::mat4x4& value ) override;

    virtual ~ShaderOpenGL() override;
}; // class Shader

class UniformBufferOpenGL : public UniformBuffer {
public:
    UniformBufferOpenGL( usize size, void* data );

public: // NOTE: virtual
    virtual void BufferData( usize size, void* data ) override;
    virtual void BufferSubData( usize offset, usize size, void* data ) override;
    virtual void SetBindingPoint( usize point ) override;
    virtual void SetBindingPointRange( usize offset, usize size, usize point ) override;
    virtual ~UniformBufferOpenGL() override;
}; // class Buffer

class VertexBufferOpenGL : public VertexBuffer {
public:
    VertexBufferOpenGL( usize size, const void* data );
    virtual ~VertexBufferOpenGL() override;

    virtual void UseBuffer() override;
    virtual void SetLayout( BufferLayout layout ) override;

}; // class Vertex Buffer

class IndexBufferOpenGL : public IndexBuffer {
public:
    IndexBufferOpenGL( BufferDataType type, usize count, const void* data );
    virtual ~IndexBufferOpenGL() override;

    virtual void UseBuffer() override;

}; // class Index Buffer

class VertexArrayOpenGL : public VertexArray {
public:
    VertexArrayOpenGL();
    virtual ~VertexArrayOpenGL() override;

    virtual void UseArray() override;
    virtual void Unbind() override;
    virtual void AddVertexBuffer( VertexBuffer* vertexBuffer ) override;
    virtual void SetIndexBuffer( IndexBuffer* indexBuffer ) override;
};

} // namespace Platform

