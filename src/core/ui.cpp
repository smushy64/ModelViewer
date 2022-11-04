#include "ui.hpp"
#include "global.hpp"
#include "utils.hpp"
#include "platform/pointer.hpp"
#include "platform/renderer.hpp"
using namespace Core::UI;

Label::Label( const std::string& text, const Core::FontAtlas* font )
: m_text(text), m_fontPtr(font) {}
LabelButton::LabelButton( const std::string& text, const Core::FontAtlas* font )
: m_text(text), m_fontPtr(font) {}
void LabelButton::UpdateState( Core::Input& input ) {
    if( input.mouseButtons[(usize)Core::MouseCode::RIGHT] ) {
        m_elementState = UI::ElementState::NORMAL;
        return;
    }

    glm::vec2 mousePosition = input.screenSpaceMouse;
    mousePosition.y = 1.0f - mousePosition.y;
    bool isHovering = Core::UI::PointInBoundingBox(
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
void LabelButton::UpdateBounds( const glm::vec2& resolution ) {
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

    UpdateScreenSpaceBounds( resolution );
}
void LabelButton::UpdateScreenSpaceBounds( const glm::vec2& resolution ) {
    m_screenSpaceBoundingBox = glm::vec4(
        m_screenSpacePosition.x + ( m_boundingBoxOffset.x / resolution.x ),
        m_screenSpacePosition.y + ( m_boundingBoxOffset.y / resolution.y ),
        m_boundingBox.x / resolution.x,
        m_boundingBox.y / resolution.y
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
void Canvas::OnResolutionChange( const glm::vec2& resolution ) {
    for( auto& labelButton : m_labelButtons ) {
        labelButton.UpdateBounds( resolution );
    }
}
void Canvas::UpdateState( Core::Input& input ) {
    for( auto& labelButton : m_labelButtons ) {
        labelButton.UpdateState( input );
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

Interface::Interface() {
    auto versionLabel = Core::UI::Label( Utils::GetProgramTitle(), nullptr );
    versionLabel.SetAnchorY( Core::YAnchor::BOTTOM );
    versionLabel.SetPosition( glm::vec2(0.01f, 0.0125f) );
    versionLabel.SetScale( 0.35f );
    m_canvas.PushLabel( versionLabel );

    f32 menuXOffset = 0.99f;
    f32 menuScale = 0.65f;

    auto loadAlbedoButton = Core::UI::LabelButton( "Load Albedo Texture", nullptr );
    loadAlbedoButton.SetAnchorX( Core::XAnchor::RIGHT );
    loadAlbedoButton.SetPosition( glm::vec2( menuXOffset, 0.25f ) );
    loadAlbedoButton.SetScale( menuScale );
    m_loadAlbedoIndex = m_canvas.PushLabelButton( loadAlbedoButton );

    auto loadSpecularButton = Core::UI::LabelButton( "Load Specular Texture", nullptr );
    loadSpecularButton.SetAnchorX( Core::XAnchor::RIGHT );
    loadSpecularButton.SetPosition( glm::vec2( menuXOffset, 0.2f ) );
    loadSpecularButton.SetScale( menuScale );
    m_loadSpecularIndex = m_canvas.PushLabelButton( loadSpecularButton );

    auto loadNormalButton = Core::UI::LabelButton( "Load Normal Texture", nullptr );
    loadNormalButton.SetAnchorX( Core::XAnchor::RIGHT );
    loadNormalButton.SetPosition( glm::vec2( menuXOffset, 0.15f ) );
    loadNormalButton.SetScale( menuScale );
    m_loadNormalIndex = m_canvas.PushLabelButton( loadNormalButton );

    auto loadMeshButton = Core::UI::LabelButton( "Load Mesh", nullptr );
    loadMeshButton.SetAnchorX( Core::XAnchor::RIGHT );
    loadMeshButton.SetPosition( glm::vec2( menuXOffset, 0.1f ) );
    loadMeshButton.SetScale( menuScale );
    m_loadMeshIndex = m_canvas.PushLabelButton( loadMeshButton );

    auto quitButton = Core::UI::LabelButton( "Quit Program", nullptr );
    quitButton.SetAnchorX( Core::XAnchor::RIGHT );
    quitButton.SetPosition( glm::vec2( menuXOffset, 0.05f ) );
    quitButton.SetScale( menuScale );
    m_quitIndex = m_canvas.PushLabelButton( quitButton );
}
Interface::~Interface() { }
void Interface::SetFont( const Core::FontAtlas* font ) {
    for( auto& label : m_canvas.GetLabelsMut() ) {
        label.SetFont( font );
    }
    for( auto& labelButton : m_canvas.GetLabelButtonsMut() ) {
        labelButton.SetFont( font );
    }
}
void Interface::SetLoadMeshCallback( ButtonCallback callback, void* params ) {
    m_canvas.GetLabelButton( m_loadMeshIndex ).SetCallback( callback );
    m_canvas.GetLabelButton( m_loadMeshIndex ).SetCallbackParameter( params );
}
void Interface::SetLoadAlbedoCallback( ButtonCallback callback, void* params ) {
    m_canvas.GetLabelButton( m_loadAlbedoIndex ).SetCallback( callback );
    m_canvas.GetLabelButton( m_loadAlbedoIndex ).SetCallbackParameter( params );
}
void Interface::SetLoadSpecularCallback( ButtonCallback callback, void* params ) {
    m_canvas.GetLabelButton( m_loadSpecularIndex ).SetCallback( callback );
    m_canvas.GetLabelButton( m_loadSpecularIndex ).SetCallbackParameter( params );
}
void Interface::SetLoadNormalCallback( ButtonCallback callback, void* params ) {
    m_canvas.GetLabelButton( m_loadNormalIndex ).SetCallback( callback );
    m_canvas.GetLabelButton( m_loadNormalIndex ).SetCallbackParameter( params );
}
void Interface::SetQuitCallback( ButtonCallback callback, void* params ) {
    m_canvas.GetLabelButton( m_quitIndex ).SetCallback( callback );
    m_canvas.GetLabelButton( m_quitIndex ).SetCallbackParameter( params );
}
void Interface::UpdateState( Core::Input& userInput ) {
    m_canvas.UpdateState( userInput );
}
void Interface::OnResolutionChange( const glm::vec2& resolution ) {
    m_canvas.OnResolutionChange( resolution );
}

bool Core::UI::PointInBoundingBox( const glm::vec2& point, const glm::vec4& bounds ) {

    if( point.x < bounds.x ) {
        return false;
    } else if( point.y < bounds.y ) {
        return false;
    } else if( point.x > bounds.x + bounds.z ) {
        return false;
    } else if( point.y > bounds.y + bounds.w ) {
        return false;
    }

    return true;

}
