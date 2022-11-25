/**
 * Description:  User interface
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 19, 2022 
 */
#pragma once
#include "pch.hpp"
#include "util.hpp"

// forward declarations
namespace Platform {
    struct RendererAPI;
};

namespace Core {

// forward declarations
struct FontAtlas;
struct RenderContext;
struct Input;

enum class Anchor {
    LEFT_CENTER,
    CENTER_CENTER,
    RIGHT_CENTER,
    LEFT_TOP,
    CENTER_TOP,
    RIGHT_TOP,
    LEFT_BOTTOM,
    CENTER_BOTTOM,
    RIGHT_BOTTOM
};

typedef void (*ButtonCallbackFN)(void* params);

/// @brief Check if a given point is within given bounds
/// @param point 2d point
/// @param bounds xy - bounds position, zw - bounds dimensions
/// @return true if point is in bounds
bool pointInBounds( const smath::vec2& point, const smath::vec4& bounds );

class Element {
public:
    Element() {}
    Element( const smath::vec2& screenSpacePosition, Anchor anchor ) :
        m_screenSpacePosition( screenSpacePosition ), m_anchor(anchor) {}
public: // getters
    Anchor anchor() const { return m_anchor; }
    const smath::vec2& screenSpacePosition() const { return m_screenSpacePosition; }
public: // setters
    void setAnchor( Anchor anchor ) { m_anchor = anchor; }
    void setScreenSpacePosition( const smath::vec2& newPos ) { m_screenSpacePosition = newPos; }
protected:
    smath::vec2 m_screenSpacePosition;
    Anchor      m_anchor;
};

class Label : public Element {
public:
    Label(
        usize textLen,
        const char* text,
        const smath::vec2& screenSpacePosition,
        const smath::vec4& color,
        f32 scale,
        Anchor anchor,
        const Core::FontAtlas* fontAtlas
    );
    Label(
        const char* text,
        const smath::vec2& screenSpacePosition,
        const smath::vec4& color,
        f32 scale,
        Anchor anchor,
        const Core::FontAtlas* fontAtlas
    ) : Label( stringLen( text ) + 1, text, screenSpacePosition, color, scale, anchor, fontAtlas ) {}
    ~Label();
    // copy
    Label( const Label& other ) : Label(
        other.m_textBufferLen,
        other.m_textBuffer,
        other.m_screenSpacePosition,
        other.m_color,
        other.m_scale,
        other.m_anchor,
        other.m_fontAtlas
    ) {}
    // move
    Label( Label&& other ) noexcept : Element( other.m_screenSpacePosition, other.m_anchor ),
    m_color(other.m_color),
    m_textBufferLen( other.m_textBufferLen ),
    m_textBuffer( other.m_textBuffer ),
    m_scale(other.m_scale),
    m_fontAtlas(other.m_fontAtlas) {
        other.m_textBuffer = nullptr;
    }
    // copy assign
    Label& operator=( const Label& other ) {
        return *this = Label(other);
    }
    // move assign
    Label& operator=( Label&& other ) noexcept {
        swapPtr( m_textBuffer, other.m_textBuffer );
        return *this;
    }
    void renderLabel( const Platform::RendererAPI* api, const Core::RenderContext* ctx );
public: // getter
    const smath::vec4& color() const { return m_color; }
    const char* text() const { return m_textBuffer; }
    usize textBufferLen() const { return m_textBufferLen; }
    f32 scale() const { return m_scale; }
    const Core::FontAtlas* fontAtlas() const { return m_fontAtlas; }
public: // setter
    void setColor( const smath::vec4& newColor ) { m_color = newColor; }
    void setText( usize newTextLen, const char* newText );
    void setText( const char* newText ) { setText( stringLen(newText) + 1, newText ); }
    void setScale( f32 newScale ) { m_scale = newScale; }
    void setFont( const Core::FontAtlas* newFontAtlas ) { m_fontAtlas = newFontAtlas; }
protected:
    smath::vec4 m_color;
    usize m_textBufferLen;
    char* m_textBuffer;
    f32 m_scale;
    const Core::FontAtlas* m_fontAtlas;
};

enum class ButtonState {
    NORMAL,
    HIGHLIGHTED,
    PRESSED
};

class LabelButton : public Label {
public:
    LabelButton(
        usize textLen,
        const char* text,
        const smath::vec2& screenSpacePosition,
        const smath::vec4& normalColor,
        const smath::vec4& highlightColor,
        const smath::vec4& pressedColor,
        f32 scale,
        Anchor anchor,
        const Core::FontAtlas* fontAtlas
    );
    LabelButton(
        const char* text,
        const smath::vec2& screenSpacePosition,
        const smath::vec4& normalColor,
        const smath::vec4& highlightColor,
        const smath::vec4& pressedColor,
        f32 scale,
        Anchor anchor,
        const Core::FontAtlas* fontAtlas
    ) : LabelButton(
        stringLen(text) + 1,
        text,
        screenSpacePosition,
        normalColor,
        highlightColor,
        pressedColor,
        scale,
        anchor,
        fontAtlas
    ) {}
    void renderButton( Platform::RendererAPI* api, Core::RenderContext* ctx );
    void updateState( Core::Input* playerInput );
    void onResolutionChange( const smath::vec2* resolutionPtr );
public: // getter
    const smath::vec4& normalColor() const { return m_normalColor; }
    const smath::vec4& highlightColor() const { return m_highlightColor; }
    const smath::vec4& pressedColor() const { return m_pressedColor; }
    const smath::vec4& boundingBox() const { return m_boundingBox; }
    const smath::vec4& screenSpaceBoundingBox() const { return m_screenSpaceBoundingBox; }
    ButtonState state() const { return m_state; }
public: // setter
    void setNormalColor( const smath::vec4& newNormalColor ) { m_normalColor = newNormalColor; }
    void setHighlightColor( const smath::vec4& newHighlightColor ) { m_highlightColor = newHighlightColor; }
    void setPressedColor( const smath::vec4& newPressedColor ) { m_pressedColor = newPressedColor; }
    void setText( usize newTextLen, const char* newText );
    void setScale( f32 newScale );
    void setFont( const Core::FontAtlas* newFontAtlas );
    void setCallback( ButtonCallbackFN callback, void* callbackParams ) { m_callback = callback; m_callbackParams = callbackParams; }
private:
    void setState( ButtonState state );
    void recalculateBounds();
    void recalculateScreenSpaceBounds();
    smath::vec4 m_normalColor;
    smath::vec4 m_highlightColor;
    smath::vec4 m_pressedColor;
    smath::vec4 m_boundingBox;
    smath::vec4 m_screenSpaceBoundingBox;
    const smath::vec2* m_resolutionPtr = nullptr;
    ButtonCallbackFN m_callback = nullptr;
    void* m_callbackParams      = nullptr;
    ButtonState m_state;
};

class UserInterface {
public:
    UserInterface( const Core::FontAtlas* defaultFont );
    ~UserInterface() {}
    
    void renderInterface( Platform::RendererAPI* api, Core::RenderContext* ctx );
    void updateInterface( Core::Input* input );
    void onResolutionChange( const smath::vec2* newResolution );
public: // getters
    LabelButton& loadMeshButton() { return m_loadMesh; }
    LabelButton& loadAlbedoTextureButton() { return m_loadAlbedoTexture; }
    LabelButton& loadSpecularTextureButton() { return m_loadSpecularTexture; }
    LabelButton& loadNormalTextureButton() { return m_loadNormalTexture; }
    Label* getLabels() { return &m_versionLabel; }
    LabelButton* getLabelButtons() { return &m_loadMesh; }
private:
    Label m_versionLabel;
    usize m_labelCount = 1;

    LabelButton m_loadMesh;
    LabelButton m_loadAlbedoTexture;
    LabelButton m_loadSpecularTexture;
    LabelButton m_loadNormalTexture;
    usize m_labelButtonCount = 4;
};

} // namespace Core

