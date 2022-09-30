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

FontID Renderer::API::GetFontID( const std::string& fontName ) {
    FontID result = -1;
    if(m_fontCount == 0) {
        LOG_ERROR("Renderer > Attempted to get font while there are no fonts loaded!");
        return result;
    }

    for( isize i = 0; i < m_fontCount; i++ ) {
        if( m_fonts[i].fontName == fontName ) {
            result = i;
            break;
        }
    }

    if(result == -1) {
        LOG_ERROR("Renderer > Font \"%s\" not found!", fontName.c_str());
    }

    return result;
}
bool Renderer::API::ValidateFontID( const FontID& fontID ) {
    if( !m_fontCount ) {
        LOG_ERROR("Renderer > Attempted to set font while there are no fonts loaded!");
        return false;
    }
    if( fontID < 0 || fontID >= m_fontCount ) {
        LOG_ERROR("Renderer > Failed to set font, font id is invalid!");
        return false;
    }

    return true;
}

std::string Renderer::API::FormatFontName(const char* fontPath) {
    // TODO: rewrite this to be more sensible
    std::stringstream stream(fontPath);
    std::string segment;
    std::vector<std::string> seglist;
    
    while(std::getline( stream, segment, '/' )) {
        seglist.push_back( segment );
    }

    std::string result = seglist.back();

    Utils::EraseSubString( result, ".ttf" );
    Utils::EraseSubString( result, ".otf" );

    return result;
}
