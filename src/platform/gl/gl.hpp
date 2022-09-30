#pragma once
#include "platform/renderer.hpp"
#include <vector>
#include <string>

namespace Platform {
namespace Renderer {

class OpenGLAPI : public API {
public:
    virtual bool Initialize() override;
    virtual void ClearScreen() override;
    virtual void SetViewport( const glm::vec2& viewport ) override;
    virtual void SetClearColor(const glm::vec4& clearColor) override;
    virtual void RenderText( const std::string& text ) override;
    virtual bool GetUniform( RendererID shaderID, const std::string& uniformName, UniformID& uniform ) override;
    virtual void UseFont( FontID fontID ) override;
    virtual void SetTextColor( const glm::vec4& color ) override;

    virtual FontID LoadFont( const char* fontPath, const Core::Text::AtlasSettings& settings ) override;

private:
    void RenderCharacter( const Core::Text::CharMetrics& character, f32 originX, f32 yOffset);
    bool CompileShader( const char* source, i32 sourceLen, u32 shaderType, RendererID& shaderID );
    bool LinkShaders( RendererID shaders[], usize count, RendererID& programID );
    RendererID m_fontProgram;
    RendererID m_fontVAO;
};

} // namespace Renderer
} // namespace Platform
