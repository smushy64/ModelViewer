#include "ui.hpp"
#include "debug.hpp"
#include "collision.hpp"
#include "global.hpp"
#include "platform/pointer.hpp"
#include "platform/renderer.hpp"
using namespace Core::UI;

Label::Label( const std::string& text, const Core::FontAtlas& font )
: m_text(text), m_fontPtr(&font) {}
LabelButton::LabelButton( const std::string& text, const Core::FontAtlas& font, const glm::vec2* resolution )
: m_text(text), m_fontPtr(&font), m_resolution(resolution) {
    UpdateBounds();
}
void LabelButton::UpdateState( Core::Input& input ) {
    if( input.mouseButtons[(usize)Core::MouseCode::RIGHT] ) {
        m_elementState = UI::ElementState::NORMAL;
        return;
    }

    glm::vec2 mousePosition = input.screenSpaceMouse;
    mousePosition.y = 1.0f - mousePosition.y;
    bool isHovering = Core::PointInBoundingBox(
        mousePosition,
        m_screenSpaceBoundingBox
    );

    UI::ElementState lastState = m_elementState;
    
    if( isHovering ) {
        Platform::SetPointerStyle( Platform::PointerStyle::HAND );
        if( input.mouseButtons[(usize)Core::MouseCode::LEFT] ) {
            m_elementState = UI::ElementState::PRESSED;
            if( lastState != m_elementState ) {
                if( m_callback != nullptr ) {
                    // set mouse to false so that it stops registering clicks
                    input.mouseButtons[(usize)Core::MouseCode::LEFT] = false;
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
        pixelScale.x += (f32)character.advance * m_scale;

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
        m_screenSpacePosition.x + ( m_boundingBoxOffset.x / m_resolution->x ),
        m_screenSpacePosition.y + ( m_boundingBoxOffset.y / m_resolution->y ),
        m_boundingBox.x / m_resolution->x,
        m_boundingBox.y / m_resolution->y
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

Canvas::Canvas() { }
Canvas::~Canvas() { }
void Canvas::OnResolutionChange( const glm::vec2& ) {
    for( auto& labelButton : m_labelButtons ) {
        labelButton.UpdateBounds();
    }
}
void Canvas::UpdateState( Core::Input& input ) {
    for( auto& labelButton : m_labelButtons ) {
        labelButton.UpdateState( input );
    }
}
void Canvas::Render( Platform::Renderer* renderer ) {
    for( auto& label : m_labels ) {
        renderer->RenderText( label );
    }
    for( auto& labelButton : m_labelButtons ) {
        renderer->RenderTextButton( labelButton );
    }
}
usize Canvas::PushLabel( Label label ) {
    usize last = m_labels.size();
    m_labels.push_back(label);
    return last;
}
usize Canvas::PushLabelButton( LabelButton labelButton ) {
    usize last = m_labelButtons.size();
    m_labelButtons.push_back(labelButton);
    return last;
}
Label& Canvas::GetLabel(usize index) {
    return m_labels[index];
}
LabelButton& Canvas::GetLabelButton(usize index) {
    return m_labelButtons[index];
}
