#include "dx11.hpp"
#ifdef WINDOWS
#include <d3d11.h>
#include "dx11Init.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "macro.hpp"

using namespace Platform;

bool RendererAPIDirectX11::Initialize( void* initData ) {
    DirectX11InitData* data = (DirectX11InitData*)initData;
    HRESULT hResult = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
#ifdef DEBUG
        D3D11_CREATE_DEVICE_DEBUG,
#else
        0,
#endif
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &data->sd,
        &m_pSwap,
        &m_pDevice,
        nullptr,
        &m_pContext
    );
    if( hResult != S_OK ) {
        LOG_ERROR("DirectX 11 > Failed to create device and swap chain!");
        return false;
    }

    ID3D11Resource* pBackBuffer = nullptr;
    hResult = m_pSwap->GetBuffer( 0, __uuidof( ID3D11Resource ), (void**)&pBackBuffer );
    if( hResult != S_OK ) {
        LOG_ERROR("DirectX 11 > Failed to get backbuffer!");
        return false;
    }
    hResult = m_pDevice->CreateRenderTargetView(
        pBackBuffer,
        nullptr,
        &m_pBackBufferTargetView
    );
    if( hResult != S_OK ) {
        LOG_ERROR("DirectX 11 > Failed to create render target view on back buffer!");
        COM_RELEASE( pBackBuffer );
        return false;
    }
    COM_RELEASE( pBackBuffer );

    return true;
}
RendererAPIDirectX11::~RendererAPIDirectX11() {
    COM_RELEASE( m_pBackBufferTargetView );
    COM_RELEASE( m_pContext );
    COM_RELEASE( m_pSwap );
    COM_RELEASE( m_pDevice );
}
void RendererAPIDirectX11::ClearBuffer() {
    m_pContext->ClearRenderTargetView(
        m_pBackBufferTargetView,
        glm::value_ptr( m_clearColor )
    );
}
void RendererAPIDirectX11::SwapBuffers() {
    const u32 SYNC_INTERVAL = 1;
    const u32 FLAGS         = 0;
    m_pSwap->Present( SYNC_INTERVAL, FLAGS );
}
void RendererAPIDirectX11::SetClearColor( const glm::vec4& color )  {
    m_clearColor = color;
}
void RendererAPIDirectX11::SetViewport( const glm::vec2& viewport ) {
    
}
void RendererAPIDirectX11::EnableWireframe() {
    
}
void RendererAPIDirectX11::DisableWireframe() {
    
}
void RendererAPIDirectX11::EnableBlending() {
    
}
void RendererAPIDirectX11::DisableBlending() {
    
}
void RendererAPIDirectX11::BlendEquation( BlendEq Color, BlendEq Alpha ) {
    
}
void RendererAPIDirectX11::SetConstantBlendColor( const glm::vec4& blendColor ) {
    
}
void RendererAPIDirectX11::SetPackAlignment( PixelAlignment alignment ) {
    
}
void RendererAPIDirectX11::SetUnpackAlignment( PixelAlignment alignment ) {
    
}
void RendererAPIDirectX11::SetActiveTexture( u32 activeTexture ) const {
    
}
void RendererAPIDirectX11::DrawVertexArray( const VertexArray* va ) {
    
}
void RendererAPIDirectX11::BlendFunction(
    BlendFactor srcColor,
    BlendFactor dstColor,
    BlendFactor srcAlpha,
    BlendFactor dstAlpha
) {
    
}

#endif
