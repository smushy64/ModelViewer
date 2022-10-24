#include "ui.hpp"
#include "debug.hpp"
#include "collision.hpp"
#include "global.hpp"
using namespace Core::UI;

Label::Label( const std::string& text, const Core::FontAtlas& font )
: m_text(text), m_fontPtr(&font) {}

LabelButton::LabelButton( const std::string& text, const Core::FontAtlas& font )
: m_text(text), m_fontPtr(&font) {
    UpdateBounds();
}

void LabelButton::UpdateState( const Core::Input& input ) {
    glm::vec2 mousePosition = input.screenSpaceMouse;
    mousePosition.y = 1.0f - mousePosition.y;
    bool isHovering = Core::PointInBoundingBox(
        mousePosition,
        m_screenSpaceBoundingBox
    );

    UI::ElementState lastState = m_elementState;
    
    if( isHovering ) {
        if( input.mouseButtons[(usize)Core::MouseCode::LEFT] ) {
            m_elementState = UI::ElementState::PRESSED;
            if( lastState != m_elementState ) {
                if( m_callback != nullptr ) {
                    m_callback(m_callbackParameter);
            }
            }
        } else {
            m_elementState = UI::ElementState::HOVERED;
        }
    } else {
        m_elementState = UI::ElementState::NORMAL;
    }
}

void LabelButton::UpdateBounds() {
    if( m_fontPtr == nullptr ) {
        LOG_WARN("UI > Could not calculate bounding box for Label Button, font pointer is null!");
        return;
    }
    glm::vec2 pixelScale = glm::vec2(0.0f);

    std::string::const_iterator iter;
    for( iter = m_text.begin(); iter < m_text.end(); iter++ ) {
        if( m_fontPtr->characterMetrics.count(*iter) == 0 ) {
            // skip characters not found in character map
            LOG_WARN("UI > Character \'%c\' not found in font \"%s\"",
                *iter, m_fontPtr->name.c_str()
            );
            continue;
        }
        auto character = m_fontPtr->characterMetrics.at( *iter );

        pixelScale.x += (character.width * m_scale) + (character.leftBearing * m_scale);

        f32 height = character.height * m_scale;
        if( pixelScale.y < height ) {
            pixelScale.y = height;
        }
    }

    m_boundingBox = pixelScale;

    switch( m_anchorX ) {
        case Core::XAnchor::CENTER: {
            m_boundingBoxOffset.x = -(pixelScale.x / 2.0f);
        } break;
        case Core::XAnchor::RIGHT: {
            m_boundingBoxOffset.x = -pixelScale.x;
        } break;
        default: {
            m_boundingBoxOffset.x = 0.0f;
        } break;
    }

    switch( m_anchorY ) {
        case Core::YAnchor::CENTER: {
            m_boundingBoxOffset.y = -( (m_fontPtr->pointSize / 2.0f) * m_scale );
        } break;
        case Core::YAnchor::TOP: {
            m_boundingBoxOffset.y = -( m_fontPtr->pointSize * m_scale );
        } break;
        default: {
            m_boundingBoxOffset.y = 0.0f;
        } break;
    }

    UpdateScreenSpaceBounds();
}

void LabelButton::UpdateScreenSpaceBounds() {
    m_screenSpaceBoundingBox = glm::vec4(
        m_screenSpacePosition.x + ( m_boundingBoxOffset.x / SCREEN_WIDTH ),
        m_screenSpacePosition.y + ( m_boundingBoxOffset.y / SCREEN_HEIGHT ),
        m_boundingBox.x / SCREEN_WIDTH,
        m_boundingBox.y / SCREEN_HEIGHT
    );
}

const char* Core::UI::ElementStateToString( ElementState state ) {
    switch( state ) {
        case ElementState::NORMAL:  return "NORMAL";
        case ElementState::HOVERED: return "HOVERED";
        case ElementState::PRESSED: return "PRESSED";
        default: return "UNKNOWN";
    }
}
