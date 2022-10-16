#pragma once
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include "core/text.hpp"
#include "consts.hpp"
#include "alias.hpp"

namespace Platform
{
typedef void (*OpenGLSwapBuffer)(void);
typedef void* (*OpenGLLoader)(const char* functionName);

class Shader {

public:

    // Create new shader from vertex + fragment source
    static Shader* New( const std::string& vertex, const std::string& fragment );

    RendererID GetID() { return m_id; }

public: // virtual
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

    const glm::vec2&     GetTextPosition() { return m_textPosition; }
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

    // Sets internal font atlas pointer to this font atlas
    void UseFontAtlas( const Core::FontAtlas& fontAtlas );
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
    void SetTextPosition( const glm::vec2& position );

protected:
    virtual void RenderCharacter( const Core::CharMetrics& metrics, const glm::vec2& origin ) = 0;

    glm::vec4 m_clearColor = glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f );
    glm::vec2 m_viewport   = glm::vec2( DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT );

    const Core::FontAtlas* m_fontAtlas = nullptr;
    glm::vec4 m_textColor        = glm::vec4( 1.0f );
    Core::XAnchor m_textXAnchor  = Core::DEFAULT_X_ANCHOR;
    Core::YAnchor m_textYAnchor  = Core::DEFAULT_Y_ANCHOR;
    const f32 DEFAULT_TEXT_SCALE = 1.0f;
    f32 m_textScale = DEFAULT_TEXT_SCALE;
    glm::vec2 m_textPosition = glm::vec2(0.0f);

    Shader* m_fontShader = nullptr;
    UniformID m_fontColorID, m_fontProjID, m_fontTransformID, m_fontCoordsID;
public:
    OpenGLSwapBuffer OpenGLSwapBufferFn;

    virtual ~Renderer() = default;
}; // class Renderer

} // namespace Platform

