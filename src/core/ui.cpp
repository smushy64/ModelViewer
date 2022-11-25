/**
 * Description:  User interface
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 19, 2022 
 */
#include "core/ui.hpp"
#include "platform/io.hpp"
#include "core/font.hpp"
#include "core/app.hpp"
using namespace Core;

void RenderText(
    usize textBufferLen,
    const char* textBuffer,
    smath::vec2* screenPos,
    f32 scale,
    smath::vec4* color,
    Core::Anchor alignment,
    Core::FontAtlas* fontAtlas,
    Platform::RendererAPI* api,
    Core::RenderContext* ctx
);

bool Core::pointInBounds( const smath::vec2& point, const smath::vec4& bounds ) {
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

Label::Label(
    usize textLen,
    const char* text,
    const smath::vec2& screenSpacePosition,
    const smath::vec4& color,
    f32 scale,
    Anchor anchor,
    const Core::FontAtlas* fontAtlas
) : Element( screenSpacePosition, anchor ),
    m_color(color),
    m_textBufferLen(textLen),
    m_scale(scale),
    m_fontAtlas(fontAtlas) {

    m_textBuffer = (char*)Platform::Alloc( m_textBufferLen );
    stringCopy( m_textBufferLen, text, m_textBufferLen, m_textBuffer );

}
void Label::setText( usize newTextLen, const char* newText ) {
    if( m_textBuffer ) {
        Platform::Free( m_textBuffer );
    }
    m_textBufferLen = newTextLen;
    m_textBuffer = (char*)Platform::Alloc( m_textBufferLen );
    stringCopy( m_textBufferLen, newText, m_textBufferLen, m_textBuffer );
}
void Label::renderLabel( const Platform::RendererAPI* api, const Core::RenderContext* ctx ) {
    RenderText(
        m_textBufferLen,
        m_textBuffer,
        &m_screenSpacePosition,
        m_scale,
        &m_color,
        m_anchor,
        (Core::FontAtlas*)m_fontAtlas,
        (Platform::RendererAPI*)api,
        (Core::RenderContext*)ctx
    );
}
Label::~Label() {
    if( m_textBuffer ) {
        Platform::Free( m_textBuffer );
    }
}

LabelButton::LabelButton(
    usize textLen,
    const char* text,
    const smath::vec2& screenSpacePosition,
    const smath::vec4& normalColor,
    const smath::vec4& highlightColor,
    const smath::vec4& pressedColor,
    f32 scale,
    Anchor anchor,
    const Core::FontAtlas* fontAtlas
) : Label(
        textLen,
        text,
        screenSpacePosition,
        normalColor,
        scale,
        anchor,
        fontAtlas
    ),
    m_normalColor( normalColor ),
    m_highlightColor( highlightColor ),
    m_pressedColor( pressedColor ) {

    m_state = ButtonState::NORMAL;
    recalculateBounds();

}
void LabelButton::setText( usize newTextLen, const char* newText ) {
    if( m_textBuffer ) {
        Platform::Free( m_textBuffer );
    }
    m_textBufferLen = newTextLen;
    m_textBuffer = (char*)Platform::Alloc( m_textBufferLen );
    stringCopy( m_textBufferLen, newText, m_textBufferLen, m_textBuffer );
    recalculateBounds();
}
void LabelButton::setScale( f32 newScale ) {
    m_scale = newScale;
    recalculateBounds();
}
void LabelButton::setFont( const Core::FontAtlas* newFontAtlas ) {
    m_fontAtlas = newFontAtlas;
    recalculateBounds();
}
void LabelButton::recalculateBounds() {
    smath::vec4 pixelBoundingBox = {};
    usize textBufferLenNoNull = m_textBufferLen - 1;
    ucycles( textBufferLenNoNull ) {
        char currentChar = m_textBuffer[i];
        Core::FontMetrics* charMetrics = m_fontAtlas->metrics.get( currentChar );
        if( !charMetrics ) {
            continue;
        }
        pixelBoundingBox.z += (f32)charMetrics->advance * m_scale;
        f32 height = charMetrics->height * m_scale;
        if( pixelBoundingBox.w < height ) {
            pixelBoundingBox.w = height;
        }
    }

    switch( m_anchor ) {
        case Core::Anchor::CENTER_CENTER:
        case Core::Anchor::CENTER_TOP:
        case Core::Anchor::CENTER_BOTTOM: {
            pixelBoundingBox.x = -( pixelBoundingBox.z / 2.0f );
        } break;
        case Core::Anchor::RIGHT_CENTER:
        case Core::Anchor::RIGHT_TOP:
        case Core::Anchor::RIGHT_BOTTOM: {
            pixelBoundingBox.x = -pixelBoundingBox.z;
        } break;
        default: {
            pixelBoundingBox.x = 0.0f;
        }break;
    }

    switch( m_anchor ) {
        case Core::Anchor::LEFT_CENTER:
        case Core::Anchor::CENTER_CENTER:
        case Core::Anchor::RIGHT_CENTER: {
            pixelBoundingBox.y = -( ( m_fontAtlas->pointSize / 2.0f ) * m_scale );
        } break;

        case Core::Anchor::LEFT_TOP:
        case Core::Anchor::CENTER_TOP:
        case Core::Anchor::RIGHT_TOP: {
            pixelBoundingBox.y = -( m_fontAtlas->pointSize * m_scale );
        } break;

        default: {
            pixelBoundingBox.y = 0.0f;
        }break;
    }

    m_boundingBox = pixelBoundingBox;
    recalculateScreenSpaceBounds();
}
void LabelButton::recalculateScreenSpaceBounds() {
    if( !m_resolutionPtr ) {
        return;
    }
    m_screenSpaceBoundingBox = smath::vec4(
        m_screenSpacePosition.x + ( m_boundingBox.x / (f32)m_resolutionPtr->x ),
        m_screenSpacePosition.y + ( m_boundingBox.y / (f32)m_resolutionPtr->y ),
        m_boundingBox.z / (f32)m_resolutionPtr->x,
        m_boundingBox.w / (f32)m_resolutionPtr->y
    );
}
void LabelButton::updateState( Core::Input* playerInput ) {
    smath::vec2 mouse = playerInput->screenMousePos;
    if( Core::pointInBounds( mouse, m_screenSpaceBoundingBox ) ) {
        Platform::SetCursorStyle( Platform::CursorStyle::HAND );
        if( playerInput->leftMouse ) {
            setState( ButtonState::PRESSED );
            if( m_callback ) {
                m_callback( m_callbackParams );
            }
            playerInput->leftMouse = false;
        } else {
            setState( ButtonState::HIGHLIGHTED );
        }
    } else {
        setState( ButtonState::NORMAL );
    }
}
void LabelButton::setState( ButtonState state ) {
    m_state = state;
    switch( m_state ) {
        case ButtonState::NORMAL: {
            m_color = m_normalColor;
        } break;
        case ButtonState::HIGHLIGHTED: {
            m_color = m_highlightColor;
        } break;
        case ButtonState::PRESSED: {
            m_color = m_pressedColor;
        } break;
    }
}
void LabelButton::onResolutionChange( const smath::vec2* resolutionPtr ) {
    m_resolutionPtr = resolutionPtr;
    recalculateScreenSpaceBounds();
}
void LabelButton::renderButton( Platform::RendererAPI* api, Core::RenderContext* ctx ) {
    renderLabel( api, ctx );

#if DEBUG

    api->SetWireframeEnabled(true);

    api->UseShader( &ctx->boundsShader );
    api->UseVertexArray( &ctx->boundsVertexArray );

    smath::vec4 bounds = smath::vec4(
        (m_screenSpacePosition.x * (f32)ctx->viewport.x) + m_boundingBox.x,
        (m_screenSpacePosition.y * (f32)ctx->viewport.y) + m_boundingBox.y,
        m_boundingBox.z,
        m_boundingBox.w
    );
    
    smath::mat4 boundsTransform =
        smath::mat4::translate( smath::vec3( bounds.x, bounds.y, 0.0f ) ) *
        smath::mat4::scale( smath::vec3( bounds.z, bounds.w, 0.0f ) );


    api->UniformMat4(
        &ctx->boundsShader,
        ctx->boundsShaderUniformTransform,
        &boundsTransform
    );

    api->DrawVertexArray( &ctx->boundsVertexArray );
    api->SetWireframeEnabled(false);

#endif
}


f32 LABEL_BUTTON_XPOS = 0.98f;
f32 LABEL_BUTTON_YPOS = 0.2f;
f32 LABEL_BUTTON_YDELTA = 0.05f;
smath::vec4 LABEL_BUTTON_NORMAL_COLOR    = smath::vec4::one();
smath::vec4 LABEL_BUTTON_HIGHLIGHT_COLOR = smath::vec4( 1.0f, 1.0f, 0.0f, 1.0f );
smath::vec4 LABEL_BUTTON_PRESSED_COLOR   = smath::vec4::one() * 0.3f;
f32 LABEL_BUTTON_SCALE = 0.6f;
Anchor LABEL_BUTTON_ANCHOR = Anchor::RIGHT_BOTTOM;
UserInterface::UserInterface( const Core::FontAtlas* defaultFont ) :
    m_versionLabel( Core::PROGRAM_TITLE_LEN,
        Core::PROGRAM_TITLE,
        smath::vec2( 0.01f, 0.0125f ),
        smath::vec4::one(),
        0.4f,
        Core::Anchor::LEFT_BOTTOM,
        defaultFont
    ),
    m_loadMesh(
        "Load Mesh",
        smath::vec2( LABEL_BUTTON_XPOS, LABEL_BUTTON_YPOS - ( LABEL_BUTTON_YDELTA * 0.0f ) ),
        LABEL_BUTTON_NORMAL_COLOR,
        LABEL_BUTTON_HIGHLIGHT_COLOR,
        LABEL_BUTTON_PRESSED_COLOR,
        LABEL_BUTTON_SCALE,
        LABEL_BUTTON_ANCHOR,
        defaultFont
    ),
    m_loadAlbedoTexture(
        "Load Albedo Texture",
        smath::vec2( LABEL_BUTTON_XPOS, LABEL_BUTTON_YPOS - ( LABEL_BUTTON_YDELTA * 1.0f ) ),
        LABEL_BUTTON_NORMAL_COLOR,
        LABEL_BUTTON_HIGHLIGHT_COLOR,
        LABEL_BUTTON_PRESSED_COLOR,
        LABEL_BUTTON_SCALE,
        LABEL_BUTTON_ANCHOR,
        defaultFont
    ),
    m_loadSpecularTexture(
        "Load Specular Texture",
        smath::vec2( LABEL_BUTTON_XPOS, LABEL_BUTTON_YPOS - ( LABEL_BUTTON_YDELTA * 2.0f ) ),
        LABEL_BUTTON_NORMAL_COLOR,
        LABEL_BUTTON_HIGHLIGHT_COLOR,
        LABEL_BUTTON_PRESSED_COLOR,
        LABEL_BUTTON_SCALE,
        LABEL_BUTTON_ANCHOR,
        defaultFont
    ),
    m_loadNormalTexture(
        "Load Normal Texture",
        smath::vec2( LABEL_BUTTON_XPOS, LABEL_BUTTON_YPOS - ( LABEL_BUTTON_YDELTA * 3.0f ) ),
        LABEL_BUTTON_NORMAL_COLOR,
        LABEL_BUTTON_HIGHLIGHT_COLOR,
        LABEL_BUTTON_PRESSED_COLOR,
        LABEL_BUTTON_SCALE,
        LABEL_BUTTON_ANCHOR,
        defaultFont
    )
{ }

void UserInterface::renderInterface( Platform::RendererAPI* api, Core::RenderContext* ctx ) {
    ucycles( m_labelCount ) {
        getLabels()[i].renderLabel( api, ctx );
    }
    ucycles( m_labelButtonCount ) {
        getLabelButtons()[i].renderButton( api, ctx );
    }
}
void UserInterface::updateInterface( Core::Input* input ) {
    ucycles( m_labelButtonCount ) {
        getLabelButtons()[i].updateState( input );
    }
}
void UserInterface::onResolutionChange( const smath::vec2* newResolution ) {
    ucycles( m_labelButtonCount ) {
        getLabelButtons()[i].onResolutionChange( newResolution );
    }
}

void RenderCharacter(
    Platform::RendererAPI* api,
    Core::RenderContext* ctx,
    Core::FontMetrics* charMetrics,
    smath::vec2* charPosition,
    smath::vec2* pixelPosition,
    f32 scale
) {
    smath::vec3 characterScale = smath::vec3( charMetrics->width, charMetrics->height, 0.0f ) * scale;
    smath::vec3 characterTranslate = smath::vec3(
        charPosition->x + ( (f32)charMetrics->leftBearing * scale ),
        ( pixelPosition->y + charPosition->y ) - ( (f32)charMetrics->topBearing * scale ),
        0.0f
    );

    smath::mat4 transform; {
        smath::mat4 translate = smath::mat4::translate( characterTranslate );
        smath::mat4 scale     = smath::mat4::scale( characterScale );
        transform = translate * scale;
    };

    api->UniformMat4(
        &ctx->fontShader,
        ctx->fontShaderUniformTransform,
        &transform
    );
    smath::vec4 fontCoords = smath::vec4(
        charMetrics->atlasX,
        charMetrics->atlasY,
        charMetrics->atlasW,
        charMetrics->atlasH
    );
    api->UniformVec4(
        &ctx->fontShader,
        ctx->fontShaderUniformFontCoords,
        &fontCoords
    );
    api->DrawVertexArray( &ctx->fontVertexArray );

}
void RenderText(
    usize textBufferLen,
    const char* textBuffer,
    smath::vec2* screenPos,
    f32 scale,
    smath::vec4* color,
    Core::Anchor alignment,
    Core::FontAtlas* fontAtlas,
    Platform::RendererAPI* api,
    Core::RenderContext* ctx
) {
    usize textBufferLenNoNull = textBufferLen - 1;

    api->SetBlendFunction(
        Platform::BlendFactor::SRC_ALPHA,
        Platform::BlendFactor::ONE_MINUS_SRC_ALPHA,
        Platform::BlendFactor::SRC_ALPHA,
        Platform::BlendFactor::ONE_MINUS_SRC_ALPHA
    );

    api->UseShader( &ctx->fontShader );
    api->UniformVec4(
        &ctx->fontShader,
        ctx->fontShaderUniformColor,
        color
    );
    api->UseTexture2D(
        &ctx->fontAtlasTexture,
        RENDER_CONTEXT_FONT_TEXTURE_UNIT
    );
    api->UseVertexArray( &ctx->fontVertexArray );

    smath::vec2 pixelPosition = smath::vec2(
        screenPos->x * (f32)ctx->viewport.x,
        screenPos->y * (f32)ctx->viewport.y
    );

    f32 originX = pixelPosition.x;
    f32 yOffset = 0.0f;
    f32 textWidth = 0.0f;
    ucycles( textBufferLenNoNull ) {
        char currentChar = textBuffer[i];
        Core::FontMetrics* charMetrics = fontAtlas->metrics.get( currentChar );
        if( !charMetrics ) {
            LOG_WARN("App > Character \'%c\' not found in font \"%s\"!",
                currentChar, fontAtlas->fontName
            );
            continue;
        }
        textWidth += charMetrics->advance * scale;
    }
    switch( alignment ) {
        case Core::Anchor::CENTER_CENTER:
        case Core::Anchor::CENTER_TOP:
        case Core::Anchor::CENTER_BOTTOM: {
            originX -= textWidth / 2.0f;
        } break;
        case Core::Anchor::RIGHT_CENTER:
        case Core::Anchor::RIGHT_TOP:
        case Core::Anchor::RIGHT_BOTTOM: {
            originX -= textWidth;
        } break;
        default: break;
    }

    switch( alignment ) {
        case Core::Anchor::LEFT_CENTER:
        case Core::Anchor::CENTER_CENTER:
        case Core::Anchor::RIGHT_CENTER: {
            yOffset = -( ( fontAtlas->pointSize / 2.0f ) * scale );
        } break;
        case Core::Anchor::LEFT_TOP:
        case Core::Anchor::CENTER_TOP:
        case Core::Anchor::RIGHT_TOP: {
            yOffset = -( fontAtlas->pointSize * scale );
        } break;
        default: break;
    }

    ucycles( textBufferLenNoNull ) {
        char currentChar = textBuffer[i];
        Core::FontMetrics* charMetrics = fontAtlas->metrics.get( currentChar );
        if( !charMetrics ) {
            LOG_WARN("App > Character \'%c\' not found in font \"%s\"!",
                currentChar, fontAtlas->fontName
            );
            continue;
        }
        smath::vec2 charPosition = smath::vec2( originX, yOffset );
        RenderCharacter(
            api, ctx,
            charMetrics,
            &charPosition,
            &pixelPosition,
            scale
        );
        originX += charMetrics->advance * scale;
    }

}
