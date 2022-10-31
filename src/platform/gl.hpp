#pragma once
#include "renderer.hpp"

namespace Platform {

u32 BufferDataTypeToGLenum( BufferDataType dataType );

class RendererAPIOpenGL : public RendererAPI {
public:
    static u32 BlendEqToGLenum( BlendEq glenum );
    static u32 BlendFactorToGLenum( BlendFactor glenum );

public: // virtual
    virtual void Initialize()  override;
    virtual void ClearBuffer() override;
    virtual void SwapBuffers() override;
    virtual void SetClearColor( const glm::vec4& color )  override;
    virtual void SetViewport( const glm::vec2& viewport ) override;
    virtual void EnableWireframe() override;
    virtual void DisableWireframe() override;
    virtual void EnableBlending() override;
    virtual void DisableBlending() override;
    virtual void BlendFunction(
        BlendFactor srcColor,
        BlendFactor dstColor,
        BlendFactor srcAlpha,
        BlendFactor dstAlpha
    ) override;
    virtual void BlendEquation( BlendEq Color, BlendEq Alpha ) override;
    virtual void SetConstantBlendColor( const glm::vec4& blendColor ) override;
    virtual void SetPackAlignment( PixelAlignment alignment ) override;
    virtual void SetUnpackAlignment( PixelAlignment alignment ) override;
    virtual void SetActiveTexture( u32 activeTexture ) const override;
    virtual void DrawVertexArray( const VertexArray* va ) override;

    virtual bool LoadOpenGLFunctions( OpenGLLoader loader ) override;

    virtual ~RendererAPIOpenGL() override;
}; // class Renderer API OpenGL

class ShaderOpenGL : public Shader {
public:
    ShaderOpenGL( const std::string& vertex, const std::string& fragment );

public: // NOTE: virtual
    virtual void UseShader() const override;
    virtual bool GetUniform( const std::string& uniformName, UniformID& id ) const override;

    virtual void UniformFloat( const UniformID& id, f32 value )               override;
    virtual void UniformInt( const UniformID& id, i32 value )                 override;
    virtual void UniformVec2( const UniformID& id, const glm::vec2& value )   override;
    virtual void UniformVec3( const UniformID& id, const glm::vec3& value )   override;
    virtual void UniformVec4( const UniformID& id, const glm::vec4& value )   override;
    virtual void UniformMat4( const UniformID& id, const glm::mat4x4& value ) override;
    virtual void UniformMat3( const UniformID& id, const glm::mat3x3& value ) override;

    virtual ~ShaderOpenGL() override;
}; // class Shader

class UniformBufferOpenGL : public UniformBuffer {
public:
    UniformBufferOpenGL( usize size, const void* data );

public: // NOTE: virtual
    virtual void BufferData( usize size, const void* data ) override;
    virtual void BufferSubData( usize offset, usize size, const void* data ) override;
    virtual void SetBindingPoint( usize point ) override;
    virtual void SetBindingPointRange( usize offset, usize size, usize point ) override;
    virtual ~UniformBufferOpenGL() override;
}; // class Buffer

class VertexBufferOpenGL : public VertexBuffer {
public:
    VertexBufferOpenGL( usize size, const void* data );
    virtual ~VertexBufferOpenGL() override;

    virtual void UseBuffer() const override;
    virtual void SetLayout( BufferLayout layout ) override;

}; // class Vertex Buffer

class IndexBufferOpenGL : public IndexBuffer {
public:
    IndexBufferOpenGL( BufferDataType type, usize count, const void* data );
    virtual ~IndexBufferOpenGL() override;

    virtual void UseBuffer() const override;

}; // class Index Buffer

class VertexArrayOpenGL : public VertexArray {
public:
    VertexArrayOpenGL();
    virtual ~VertexArrayOpenGL() override;

    virtual void UseArray() const override;
    virtual void Unbind() const override;
    virtual void AddVertexBuffer( VertexBuffer* vertexBuffer ) override;
    virtual void SetIndexBuffer( IndexBuffer* indexBuffer ) override;
}; // class Vertex Array

u32 TextureFormatToGLenum( TextureFormat format );
i32 TextureInternalFormatToGLint( TextureInternalFormat format );
i32 TextureWrapModeToGLint( TextureWrapMode wrap );
i32 TextureFilterMinToGLint( TextureFilterMin filter );
i32 TextureFilterMagToGLint( TextureFilterMag filter );

class Texture2DOpenGL : public Texture2D {
public:
    Texture2DOpenGL(
        const glm::ivec2& dimensions,
        const void* data,
        TextureFormat format,
        TextureInternalFormat internalFormat,
        BufferDataType dataType,
        i32  mipmapLevel,
        bool storeData
    );
    virtual ~Texture2DOpenGL() override;
    virtual void UseTexture() const override;
    virtual void SetHorizontalWrap( TextureWrapMode wrap ) override;
    virtual void SetVerticalWrap( TextureWrapMode wrap ) override;
    virtual void SetMinificationFilter( TextureFilterMin filter ) override;
    virtual void SetMagnificationFilter( TextureFilterMag filter ) override;
}; // class Texture 2D

} // namespace Platform

