#pragma once
#include <string>
#include "text.hpp"
#include "input.hpp"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace Core {
namespace UI {

inline const glm::vec2     DEFAULT_TEXT_POSITION = glm::vec2(0.0f);
inline const glm::vec4     DEFAULT_TEXT_COLOR    = glm::vec4(1.0f);
inline const Core::XAnchor DEFAULT_TEXT_X_ANCHOR = Core::XAnchor::LEFT;
inline const Core::YAnchor DEFAULT_TEXT_Y_ANCHOR = Core::YAnchor::BOTTOM;
inline const f32           DEFAULT_TEXT_SCALE    = 1.0f;
inline const glm::vec4     DEFAULT_HOVER_TEXT_COLOR = glm::vec4( 0.75f, 0.75f, 0.75f, 1.0f );
inline const glm::vec4     DEFAULT_PRESS_TEXT_COLOR = glm::vec4( 0.4f, 0.4f, 0.4f, 1.0f );

enum class ElementState {
    NORMAL,
    HOVERED,
    PRESSED
};

const char* ElementStateToString( ElementState state );

typedef void (*ButtonCallback)();

class Label {
public:
    Label( const std::string& text, const Core::FontAtlas& font );
public: // NOTE: Getters
    f32                Scale()               const { return m_scale; }
    Core::XAnchor      AnchorX()             const { return m_anchorX; }
    Core::YAnchor      AnchorY()             const { return m_anchorY; }
    const glm::vec2&   ScreenSpacePosition() const { return m_screenSpacePosition; }
    const glm::vec4&   Color()               const { return m_color; }
    const std::string& Text()                const { return m_text; }
    const Core::FontAtlas* Font()            const { return m_fontPtr; }

public: // NOTE: Setters
    // Set Label Scale
    void SetScale( f32 scale ) { m_scale = scale; }
    // Set Anchors
    void SetAnchors( Core::XAnchor anchorX, Core::YAnchor anchorY ) { m_anchorX = anchorX; m_anchorY = anchorY; }
    void SetAnchorX( Core::XAnchor anchorX ) { m_anchorX = anchorX; }
    void SetAnchorY( Core::YAnchor anchorY ) { m_anchorY = anchorY; }
    // Set Screen Space Position
    void SetPosition( const glm::vec2& screenSpacePosition ) { m_screenSpacePosition = screenSpacePosition; }
    // Set Color
    void SetColor( const glm::vec4& color ) { m_color = color; }
    // Set Text
    void SetText( const std::string& text ) { m_text = text; }
    // Set Font
    void SetFont( const Core::FontAtlas& font ) { m_fontPtr = &font; }
private:
    f32           m_scale   = DEFAULT_TEXT_SCALE;
    Core::XAnchor m_anchorX = DEFAULT_TEXT_X_ANCHOR;
    Core::YAnchor m_anchorY = DEFAULT_TEXT_Y_ANCHOR;
    glm::vec4     m_color   = DEFAULT_TEXT_COLOR;
    glm::vec2     m_screenSpacePosition = DEFAULT_TEXT_POSITION;
    std::string   m_text    = "";

    const Core::FontAtlas* m_fontPtr = nullptr;
}; // Class Label

class LabelButton {
public:
    LabelButton( const std::string& text, const Core::FontAtlas& font );

public:
    // Update Button State
    void UpdateState( const Core::Input& input );

public: // NOTE: Getters
    f32                Scale()               const { return m_scale; }
    Core::XAnchor      AnchorX()             const { return m_anchorX; }
    Core::YAnchor      AnchorY()             const { return m_anchorY; }
    const glm::vec2&   ScreenSpacePosition() const { return m_screenSpacePosition; }
    const glm::vec4&   BaseColor()           const { return m_baseColor; }
    const glm::vec4&   HoverColor()          const { return m_hoverColor; }
    const glm::vec4&   PressColor()          const { return m_pressColor; }
    const std::string& Text()                const { return m_text; }
    const Core::FontAtlas* Font()            const { return m_fontPtr; }
    const glm::vec2&   BoundingBox()         const { return m_boundingBox; }
    const glm::vec2&   BoundingBoxOffset()   const { return m_boundingBoxOffset; }
    const glm::vec4&   ScreenSpaceBounds()   const { return m_screenSpaceBoundingBox; }
    UI::ElementState   State()               const { return m_elementState; }

public: // NOTE: Setters
    // Set Label Scale
    void SetScale( f32 scale ) { m_scale = scale; }
    // Set Anchors
    void SetAnchors( Core::XAnchor anchorX, Core::YAnchor anchorY ) { m_anchorX = anchorX; m_anchorY = anchorY; UpdateBounds(); }
    void SetAnchorX( Core::XAnchor anchorX ) { m_anchorX = anchorX; UpdateBounds(); }
    void SetAnchorY( Core::YAnchor anchorY ) { m_anchorY = anchorY; UpdateBounds(); }
    // Set Screen Space Position
    void SetPosition( const glm::vec2& screenSpacePosition ) { m_screenSpacePosition = screenSpacePosition; UpdateScreenSpaceBounds(); }
    // Set Base Color
    void SetBaseColor( const glm::vec4& color ) { m_baseColor = color; }
    // Set Hover Color
    void SetHoverColor( const glm::vec4& color ) { m_hoverColor = color; }
    // Set Press Color
    void SetPressColor( const glm::vec4& color ) { m_pressColor = color; }
    // Set Font
    void SetFont( const Core::FontAtlas& font ) { m_fontPtr = &font; UpdateBounds(); }
    // Set Text
    void SetText( const std::string& text ) { m_text = text; UpdateBounds(); }
    // Set callback to run when button is pressed
    void SetCallback( ButtonCallback callback ) { m_callback = callback; }
private:
    void UpdateBounds();
    void UpdateScreenSpaceBounds();
    f32           m_scale   = DEFAULT_TEXT_SCALE;
    Core::XAnchor m_anchorX = DEFAULT_TEXT_X_ANCHOR;
    Core::YAnchor m_anchorY = DEFAULT_TEXT_Y_ANCHOR;

    glm::vec4     m_baseColor  = DEFAULT_TEXT_COLOR;
    glm::vec4     m_hoverColor = DEFAULT_HOVER_TEXT_COLOR;
    glm::vec4     m_pressColor = DEFAULT_PRESS_TEXT_COLOR;

    glm::vec2     m_screenSpacePosition = DEFAULT_TEXT_POSITION;
    std::string   m_text    = "";

    glm::vec2     m_boundingBox;
    glm::vec2     m_boundingBoxOffset;
    glm::vec4     m_screenSpaceBoundingBox;

    UI::ElementState m_elementState = UI::ElementState::NORMAL;

    const Core::FontAtlas* m_fontPtr = nullptr;

    ButtonCallback m_callback = nullptr;
}; // Class Label Button

} // namespace Core::UI

} // namespace Core
