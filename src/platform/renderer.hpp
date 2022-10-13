#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include "defines.hpp"
#include <vector>
#include <map>
#include <string>
#include "core/text.hpp"
#include "core/font.hpp"

namespace Platform {

namespace Renderer {

typedef void* (*OpenGLLoader)(const char* functionName);
bool LoadOpenGLFunctions( OpenGLLoader loader );

class API {
public:
    enum Backend {
        OPENGL
    };
public:
    static API* Create( Backend );

    virtual ~API() = default;
    virtual bool Initialize() = 0;
    virtual void SetViewport( const glm::vec2& viewport ) = 0;
    virtual void ClearScreen() = 0;
    virtual void SetClearColor(const glm::vec4& clearColor) = 0;
    virtual void RenderText( const std::string& text ) = 0;
    virtual bool GetUniform( RendererID shaderID, const std::string& uniformName, UniformID& uniform ) = 0;

    virtual void LoadFontAtlas( Core::Font::Atlas& fontAtlas );
    void UseFont( const Core::Font::Atlas& fontAtlas );

    // 0.0-1.0 RGB
    virtual void SetTextColor( const glm::vec4& color ) = 0;
    // 0.0-1.0 RGBA
    void SetTextColor( const glm::vec3& color );
    // 0.0-1.0 screen space coordinates
    void SetTextPosition( const glm::vec2& screenSpacePosition );
    void SetTextAnchor( Core::Text::AnchorHorizontal, Core::Text::AnchorVertical );
    void SetTextScale( f32 scale );

protected:

    Core::Font::Atlas const* m_fontAtlas;

    glm::vec2 m_textScreenSpacePosition = glm::vec2(0.0f);
    glm::vec2 m_textPosition            = glm::vec2(0.0f);
    f32       m_textScale               = 1.0f;
    glm::vec4 m_textColor               = glm::vec4(1.0f);
    Core::Text::AnchorHorizontal m_textAnchorHorizontal = Core::Text::AnchorHorizontal::LEFT;
    Core::Text::AnchorVertical   m_textAnchorVertical   = Core::Text::AnchorVertical::BOTTOM;

    const f32 DEFAULT_FONT_SIZE = 32.0f;
    const f32 SCALE_FACTOR = 0.2f;

    glm::vec2 m_viewport;

    UniformID m_textProjectionID;
    UniformID m_textTransformID;
    UniformID m_textColorID;
    UniformID m_textFontCoordsID;
};

} // namespace Renderer

} // namespace Platform
