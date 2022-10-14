#pragma once
#include "renderer.hpp"

namespace Platform {
    
class RendererOpenGL : public Renderer {
public: // NOTE: virtual
    virtual bool Initialize()  override;
    virtual void ClearBuffer() override;
    virtual void SwapBuffer()  override;
    virtual void SetClearColor( const glm::vec4& clearColor ) override;
    virtual void SetViewport( const glm::vec2& viewport ) override;

    virtual bool LoadOpenGLFunctions( OpenGLLoader loader ) override;
};

} // namespace Platform

