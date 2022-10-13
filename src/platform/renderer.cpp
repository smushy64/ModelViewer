#include "renderer.hpp"
#include "gl/gl.hpp"
#include "debug.hpp"
#include <sstream>
#include "utils.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace Platform;

Renderer::API* Renderer::API::Create( Renderer::API::Backend api ) {
    switch(api) {
        case Renderer::API::Backend::OPENGL:
            return new Renderer::OpenGLAPI();
        default:
            return nullptr;
    }
}

void Renderer::API::SetTextColor( const glm::vec3& color ) {
    SetTextColor( glm::vec4( color.r, color.g, color.b, 1.0f ) );
}

void Renderer::API::SetTextPosition( const glm::vec2& screenSpacePosition ) {
    glm::vec2 position = glm::vec2( m_viewport.x * screenSpacePosition.x, m_viewport.y * screenSpacePosition.y );
    m_textPosition = position;
    m_textScreenSpacePosition = screenSpacePosition;
}
void Renderer::API::SetTextScale( f32 scale ) {
    m_textScale = scale;
}
void Renderer::API::SetTextAnchor( Core::Text::AnchorHorizontal h, Core::Text::AnchorVertical v ) {
    m_textAnchorHorizontal = h;
    m_textAnchorVertical   = v;
}
void Renderer::API::UseFont( const Core::Font::Atlas& fontAtlas ) {
    m_fontAtlas = &fontAtlas;
}
