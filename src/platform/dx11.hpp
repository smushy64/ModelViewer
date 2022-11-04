#pragma once
#ifdef WINDOWS
#include "platform/renderer.hpp"

// Forward Declarations
struct ID3D11DeviceContext;
struct ID3D11Device;
struct IDXGISwapChain;
struct ID3D11Resource;
struct ID3D11RenderTargetView;

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
    ID3D11DeviceContext*    m_pContext = nullptr;
    ID3D11Device*           m_pDevice  = nullptr;
    IDXGISwapChain*         m_pSwap    = nullptr;
    ID3D11RenderTargetView* m_pBackBufferTargetView = nullptr;

}; // class RendererAPI

}

#endif