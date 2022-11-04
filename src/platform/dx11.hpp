#pragma once
#ifdef WINDOWS
#include "alias.hpp"
#include "platform/renderer.hpp"

// Forward Declarations
struct ID3D11DeviceContext;
struct ID3D11Device;
struct IDXGISwapChain;
struct ID3D11Resource;
struct ID3D11RenderTargetView;
struct ID3D11Buffer;

namespace Platform {

class RendererAPIDirectX11 : public RendererAPI {
public:
    virtual ~RendererAPIDirectX11() override;

public: // virtual
    virtual bool Initialize(void* initData)  override;

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
    virtual BackendAPI GetBackend() const override { return BackendAPI::DIRECTX11; }

protected:
    BackendAPI m_backend = BackendAPI::DIRECTX11;

private:
    ID3D11DeviceContext*    m_context   = nullptr;
    IDXGISwapChain*         m_swapchain = nullptr;
    ID3D11Device*           m_device    = nullptr;
    ID3D11RenderTargetView* m_backBufferTargetView = nullptr;

#ifdef DEBUG
    u64             m_nextInfoMessage = 0;
#endif

}; // class RendererAPI

class ShaderDirectX11 : public Shader {
public:
    ShaderDirectX11( const std::string& vertex, const std::string& fragment );
    virtual ~ShaderDirectX11() override;

public: // virtual
    virtual void UseShader() const override;
    virtual bool GetUniform( const std::string& uniformName, UniformID& id ) const override;

    virtual void UniformFloat( const UniformID& id, f32 value )               override;
    virtual void UniformInt( const UniformID& id, i32 value )                 override;
    virtual void UniformVec2( const UniformID& id, const glm::vec2& value )   override;
    virtual void UniformVec3( const UniformID& id, const glm::vec3& value )   override;
    virtual void UniformVec4( const UniformID& id, const glm::vec4& value )   override;
    virtual void UniformMat4( const UniformID& id, const glm::mat4x4& value ) override;
    virtual void UniformMat3( const UniformID& id, const glm::mat3x3& value ) override;

private:

};

class VertexArrayDirectX11 : public VertexArray {
public:
    VertexArrayDirectX11() {}
    virtual ~VertexArrayDirectX11() override;

public: // virtual
    virtual void UseArray() const override;
    virtual void Unbind() const override;
    virtual void AddVertexBuffer( VertexBuffer* vertexBuffer ) override;
    virtual void SetIndexBuffer( IndexBuffer* indexBuffer ) override;

private:

}; // class Vertex Array

class VertexBufferDirectX11 : public VertexBuffer {
public:
    VertexBufferDirectX11( usize size, const void* data );
    virtual ~VertexBufferDirectX11() override;

public: // virtual
    virtual void UseBuffer() const override;
    virtual void SetLayout( BufferLayout layout ) override;

private:
    ID3D11Buffer* m_pVertexBuffer = nullptr;
    u32* m_stride  = nullptr;
    u32* m_offsets = nullptr;

}; // class Vertex Buffer

class IndexBufferDirectX11 : public IndexBuffer {
public:
    IndexBufferDirectX11( BufferDataType type, usize count, const void* data );
    virtual ~IndexBufferDirectX11() override;
public: // virtual
    virtual void UseBuffer() const override;

private:

}; // class Index Buffer

class UniformBufferDirectX11 : public UniformBuffer {
public:
    UniformBufferDirectX11( usize size, const void* data );
    virtual ~UniformBufferDirectX11() override;

public: // virtual
    virtual void BufferData( usize size, const void* data ) override;
    virtual void BufferSubData( usize offset, usize size, const void* data ) override;
    virtual void SetBindingPoint( usize point ) override;
    virtual void SetBindingPointRange( usize offset, usize size, usize point ) override;

private:

}; // class Uniform Buffer

class Texture2DDirectX11 : public Texture2D {
public:
    Texture2DDirectX11(
        const glm::ivec2& dimensions,
        const void* data,
        TextureFormat format,
        TextureInternalFormat internalFormat,
        BufferDataType dataType,
        i32 mipmapLevel,
        bool storeData
    );
    virtual ~Texture2DDirectX11() override;

public: // virtual
    virtual void UseTexture( usize unit ) const override;
    virtual void SetHorizontalWrap( TextureWrapMode wrap ) override;
    virtual void SetVerticalWrap( TextureWrapMode wrap ) override;
    virtual void SetMinificationFilter( TextureFilterMin filter ) override;
    virtual void SetMagnificationFilter( TextureFilterMag filter ) override;
}; // class Texture2D

}

#endif