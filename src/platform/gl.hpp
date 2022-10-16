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

    virtual void SetTextColor( const glm::vec4& color ) override;
    virtual void LoadFontAtlasBitmap( Core::FontAtlas& fontAtlas ) override;
    virtual void RenderText( const std::string& text ) override;

    virtual bool LoadOpenGLFunctions( OpenGLLoader loader ) override;

protected:
    virtual void RenderCharacter( const Core::CharMetrics& metrics, const glm::vec2& origin ) override;
private:
    RendererID m_fontVAO;
};

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
};

} // namespace Platform

