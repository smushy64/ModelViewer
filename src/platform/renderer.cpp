#include "renderer.hpp"
#include "gl.hpp"
#include "debug.hpp"
#include "global.hpp"
#include "consts.hpp"

using namespace Platform;

Renderer* Renderer::New( Renderer::Backend backend ) {
    switch (backend) {
        case Renderer::Backend::OPENGL: {
            LOG_INFO("Current Backend: OpenGL Core 4.6");
            CURRENT_BACKEND = Renderer::Backend::OPENGL;
        } return new RendererOpenGL();
        default: {
            LOG_ERROR("Backend not yet implemented!");
        } return nullptr;
    }
}

void Renderer::UseFontAtlas( const Core::FontAtlas& fontAtlas ) {
    if( fontAtlas.textureID == U32_MAX ) {
        LOG_ERROR( "Renderer > Attempted to use font atlas whose texture is not in GPU!" );
        return;
    }
    m_fontAtlas = &fontAtlas;
}

void Renderer::UseFontAtlas( const Core::FontAtlas* fontAtlas ) {
    if( fontAtlas->textureID == U32_MAX ) {
        LOG_ERROR( "Renderer > Attempted to use font atlas whose texture is not in GPU!" );
        return;
    }
    m_fontAtlas = fontAtlas;
}

void Renderer::RenderText( const Core::UI::Label& label ) {
    SetTextScale( label.Scale() );
    SetTextColor( label.Color() );
    SetTextPosition( label.ScreenSpacePosition() );
    SetTextAnchors( label.AnchorX(), label.AnchorY() );
    UseFontAtlas( label.Font() );
    RenderText( label.Text() );
}

void Renderer::RenderTextButton( const Core::UI::LabelButton& button ) {
    SetTextScale( button.Scale() );
    switch( button.State() ) {
        case Core::UI::ElementState::HOVERED: SetTextColor( button.HoverColor() ); break;
        case Core::UI::ElementState::PRESSED: SetTextColor( button.PressColor() ); break;
        default: SetTextColor( button.BaseColor() ); break;
    }
    SetTextPosition( button.ScreenSpacePosition() );
    SetTextAnchors( button.AnchorX(), button.AnchorY() );
    UseFontAtlas( button.Font() );
    RenderText( button.Text() );

#ifdef DEBUG
    RenderBoundingBox( glm::vec4(
        (button.ScreenSpacePosition().x * m_viewport.x) + button.BoundingBoxOffset().x,
        (button.ScreenSpacePosition().y * m_viewport.y) + button.BoundingBoxOffset().y,
        button.BoundingBox().x,
        button.BoundingBox().y
    ) );
#endif
}

void Renderer::ResetTextParameters() {
    SetTextPosition( Core::UI::DEFAULT_TEXT_POSITION );
    SetTextScale( Core::UI::DEFAULT_TEXT_SCALE );
    SetTextColor( Core::UI::DEFAULT_TEXT_COLOR );
    SetTextAnchors( Core::UI::DEFAULT_TEXT_X_ANCHOR, Core::UI::DEFAULT_TEXT_Y_ANCHOR );
}

void Renderer::SetViewport( const glm::ivec2& viewport ) {
    SetViewport( glm::vec2( (f32)viewport.x, (f32)viewport.y ) );
}

void Renderer::SetClearColor( const glm::vec3& clearColor ) {
    SetClearColor( glm::vec4( clearColor.r, clearColor.g, clearColor.b, 1.0f ) );
}

void Renderer::SetTextColor( const glm::vec3& color ) {
    SetTextColor( glm::vec4( color.r, color.g, color.b, 1.0f ) );
}

void Renderer::SetTextXAnchor( const Core::XAnchor& anchor ) {
    m_textXAnchor = anchor;
}
void Renderer::SetTextYAnchor( const Core::YAnchor& anchor ) {
    m_textYAnchor = anchor;
}
void Renderer::SetTextAnchors( const Core::XAnchor& xAnchor, const Core::YAnchor& yAnchor ) {
    SetTextXAnchor(xAnchor);
    SetTextYAnchor(yAnchor);
}
void Renderer::SetTextScale( f32 scale ) {
    m_textScale = scale;
}
void Renderer::SetTextPosition( const glm::vec2& screenSpacePosition ) {
    m_textScreenPosition = screenSpacePosition;
    m_textPosition = glm::vec2( screenSpacePosition.x * m_viewport.x, screenSpacePosition.y * m_viewport.y );
}

Shader* Shader::New( const std::string& vertex, const std::string& fragment ) {
    switch( CURRENT_BACKEND ) {
        case Renderer::Backend::OPENGL:
            return new ShaderOpenGL( vertex, fragment );
        default:{
            LOG_ERROR("Current Backend does not yet implement shaders!");
        }return nullptr;
    }
}
