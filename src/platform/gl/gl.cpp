#include "gl.hpp"
#include "glad/glad.h"
#include "debug.hpp"
#include "platform/io.hpp"
#include <string>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_RECT_PACK_IMPLEMENTATION 1
#include "stb/stb_rect_pack.h"

#define STB_TRUETYPE_IMPLEMENTATION 1
#include "stb/stb_truetype.h"

using namespace Platform;

void Renderer::OpenGLAPI::SetViewport( const glm::vec2& viewport ) {
    glViewport( 0, 0, (GLsizei)viewport.x, (GLsizei)viewport.y );
    m_viewport = viewport;

    glm::mat4 textProjection = glm::ortho( 0.0f, m_viewport.x, 0.0f, m_viewport.y );
    glUseProgram( m_fontProgram );
    glUniformMatrix4fv( m_textProjectionID, 1, GL_FALSE, glm::value_ptr(textProjection) );
    SetTextPosition(m_textScreenSpacePosition);
}

void Renderer::OpenGLAPI::UseFont( FontID fontID ) {
    if(!ValidateFontID(fontID)) {
        return;
    }
    LOG_INFO("Renderer > Set font to \"%s\"", m_fonts[fontID].fontName.c_str());
    m_currentFont = fontID;
}
void Renderer::OpenGLAPI::SetTextColor( const glm::vec4& color ) {
    m_textColor = color;
    glUseProgram( m_fontProgram );
    glUniform4fv( m_textColorID, 1, glm::value_ptr(m_textColor) );
}

void Renderer::OpenGLAPI::ClearScreen() {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void Renderer::OpenGLAPI::SetClearColor(const glm::vec4& clearColor) {
    LOG_INFO("OpenGL > Set Clear Color to ( %.1f, %.1f, %.1f, %.1f )",
        clearColor.r, clearColor.g, clearColor.b, clearColor.a
    );
    glClearColor( clearColor.r, clearColor.g, clearColor.b, clearColor.a );
}

void Renderer::OpenGLAPI::RenderText( const std::string& text ) {
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram( m_fontProgram );

    const Core::Text::FontAtlas& currentFont = m_fonts[m_currentFont];

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, currentFont.textureID );

    glBindVertexArray( m_fontVAO );
    
    f32 originX = m_textPosition.x;
    f32 yOffset = 0.0f;
    switch(m_textAnchorHorizontal) {
        case Core::Text::AnchorHorizontal::CENTER: {
            std::string::const_iterator iter;
            f32 stringWidth = 0.0f;
            for( iter = text.begin(); iter < text.end(); iter++ ) {
                if( currentFont.characters.count(*iter) == 0 ) {
                    // skip characters not found in character map
                    LOG_WARN("OpenGL > Character \'%c\' not found in font \"%s\"",
                        *iter, currentFont.fontName.c_str()
                    );
                    continue;
                }
                auto character = currentFont.characters.at( *iter );
                stringWidth += character.advance * m_textScale;
            }
            originX -= stringWidth / 2.0f;
        } break;
        case Core::Text::AnchorHorizontal::RIGHT: {
            std::string::const_iterator iter;
            f32 stringWidth = 0.0f;
            for( iter = text.begin(); iter < text.end(); iter++ ) {
                if( currentFont.characters.count(*iter) == 0 ) {
                    // skip characters not found in character map
                    LOG_WARN("OpenGL > Character \'%c\' not found in font \"%s\"",
                        *iter, currentFont.fontName.c_str()
                    );
                    continue;
                }
                auto character = currentFont.characters.at( *iter );
                stringWidth += character.advance * m_textScale;
            }
            originX -= stringWidth;
        } break;
        default: break;
    }

    switch(m_textAnchorVertical) {
        case Core::Text::AnchorVertical::CENTER: {
            yOffset = -((currentFont.fontSize / 2.0f) * m_textScale);
        } break;
        case Core::Text::AnchorVertical::TOP: {
            yOffset = -(currentFont.fontSize * m_textScale);
        } break;
        default: break;
    }

    std::string::const_iterator iter;
    for( iter = text.begin(); iter < text.end(); iter++ ) {
        if( currentFont.characters.count(*iter) == 0 ) {
            // skip characters not found in character map
            LOG_WARN("OpenGL > Character \'%c\' not found in font \"%s\"",
                *iter, currentFont.fontName.c_str()
            );
            continue;
        }
        auto character = currentFont.characters.at( *iter );
        RenderCharacter( character, originX, yOffset );
        originX += character.advance * m_textScale;
    }

    glDisable(GL_BLEND);
}

void Renderer::OpenGLAPI::RenderCharacter( const Core::Text::CharMetrics& character, f32 originX, f32 yOffset ) {

    glm::vec3 characterScale = glm::vec3( character.width, character.height, 0.0f ) * m_textScale;
    glm::vec3 characterTranslate = glm::vec3(
        originX + (character.leftBearing * m_textScale),
        (m_textPosition.y + yOffset) - (character.topBearing * m_textScale),
        0.0f
    );

    glm::mat4 transform = glm::scale(
        glm::translate( glm::mat4(1.0f), characterTranslate ),
        characterScale
    );
    glUniformMatrix4fv( m_textTransformID, 1, GL_FALSE, glm::value_ptr(transform) );

    glm::vec4 fontCoords = glm::vec4(
        character.texCoordX,// atlas x
        character.texCoordY,// atlas y
        character.texWidth, // width
        character.texHeight // height
    );

    glUniform4fv( m_textFontCoordsID, 1, glm::value_ptr(fontCoords) );
    glDrawElements(
        GL_TRIANGLES,
        6,
        GL_UNSIGNED_INT,
        nullptr
    );
}

FontID Renderer::OpenGLAPI::LoadFont( const char* fontPath, const Core::Text::AtlasSettings& settings ) {
    
    auto fontFile = IO::LoadFile( fontPath );
    if( fontFile.contents ) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        u8* fontBytes = (u8*)fontFile.contents;

        Core::Text::FontAtlas atlas = {};
        atlas.fontName = FormatFontName(fontPath);
        atlas.scaleX   = settings.scaleX;
        atlas.scaleY   = settings.scaleY;
        atlas.fontSize = settings.fontSize;

        u8* bitmap = (u8*)malloc( atlas.scaleX * atlas.scaleY );

        stbtt_pack_context pc;
        if(!stbtt_PackBegin( &pc, bitmap, atlas.scaleX, atlas.scaleY, 0, 1, nullptr )) {
            LOG_ERROR("stbtt > Failed to begin packing!");
            free(bitmap);
            IO::FreeFile(fontFile);
            return -1;
        }
        stbtt_PackSetOversampling( &pc, 1, 1 );

        usize glyphCount = (usize)(settings.toChar - settings.fromChar) + 1;

        stbtt_packedchar glyphMetrics[glyphCount];
        stbtt_pack_range range = {};
        range.font_size                        = settings.fontSize;
        range.first_unicode_codepoint_in_range = settings.fromChar;
        range.array_of_unicode_codepoints      = nullptr;
        range.num_chars                        = (i32)glyphCount;
        range.chardata_for_range               = glyphMetrics;

        if(!stbtt_PackFontRanges( &pc, fontBytes, 0, &range, 1 )) {
            LOG_ERROR("stbtt > Failed to pack font ranges!");
            free(bitmap);
            IO::FreeFile(fontFile);
            return -1;
        }
        stbtt_PackEnd(&pc);

        // load bitmap into gl
        glGenTextures( 1, &atlas.textureID );
        glBindTexture( GL_TEXTURE_2D, atlas.textureID );
        glTexImage2D(
            GL_TEXTURE_2D,       // target
            0,                   // mipmap level
            GL_RED,              // internal format
            (i32)atlas.scaleX,   // width
            (i32)atlas.scaleY,   // height
            0,                   // border
            GL_RED,              // format
            GL_UNSIGNED_BYTE,    // data type
            bitmap               // data
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // TODO: create char metrics for each char
        stbtt_fontinfo fontInfo;
        stbtt_InitFont( &fontInfo, fontBytes, stbtt_GetFontOffsetForIndex( fontBytes, 0 ) );

        f32 em = stbtt_ScaleForMappingEmToPixels( &fontInfo, settings.fontSize );
        LOG_DEBUG("Em Pixels: %f", em);

        for( usize i = 0; i < glyphCount; i++ ) {
            char character = settings.fromChar + (char)i;
            Core::Text::CharMetrics metrics = {};
            const stbtt_packedchar& glyphMetric = glyphMetrics[i];

            metrics.width  = (f32)(glyphMetric.x1 - glyphMetric.x0);
            metrics.height = (f32)(glyphMetric.y1 - glyphMetric.y0);

            // IMPORTANT: This code is correct!!!!!!!!!
            metrics.texCoordX = (f32)(glyphMetric.x0) / (f32)atlas.scaleX;
            metrics.texCoordY = (f32)(glyphMetric.y0) / (f32)atlas.scaleY;
            metrics.texWidth  = metrics.width  / (f32)atlas.scaleX;
            metrics.texHeight = metrics.height / (f32)atlas.scaleY;


            i32 advanceWidth, leftSideBearing;
            stbtt_GetCodepointHMetrics(&fontInfo, character, &advanceWidth, &leftSideBearing);

            metrics.leftBearing = leftSideBearing * em;
            metrics.topBearing  = glyphMetric.yoff2;
            metrics.advance     = glyphMetric.xadvance;

            atlas.characters.insert( std::pair<char, Core::Text::CharMetrics>(character, metrics) );
            atlas.characterCount++;
        }

        m_fonts.push_back( atlas );

        // NOTE: cleanup
        free(bitmap);
        IO::FreeFile(fontFile);

        FontID id = m_fontCount;
        m_fontCount++;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        return id;
    } else {
        LOG_ERROR("OpenGL > Failed to load font at path \"%s\"!", fontPath);
        return -1;
    }

}

bool Renderer::OpenGLAPI::CompileShader( const char* source, i32 sourceLen, u32 shaderType, RendererID& shaderID ) {
    shaderID = glCreateShader( shaderType );
    glShaderSource(
        shaderID,
        1,
        &source,
        &sourceLen
    );
    glCompileShader( shaderID );

    i32 success;
    glGetShaderiv( shaderID, GL_COMPILE_STATUS, &success );
    if( success == GL_FALSE ) {
        i32 len = 0;
        glGetShaderiv( shaderID, GL_INFO_LOG_LENGTH, &len );
        char* buffer = (char*)malloc( len );
        glGetShaderInfoLog(
            shaderID,
            len,
            nullptr,
            buffer
        );
        LOG_ERROR("OpenGL Compilation Error > %s", buffer);
        free(buffer);
        return false;
    } else { return true; }
}
bool Renderer::OpenGLAPI::LinkShaders( RendererID shaders[], usize count, RendererID& programID ) {
    programID = glCreateProgram();
    for( usize i = 0; i < count; i++ ) {
        glAttachShader( programID, shaders[i] );
    }
    glLinkProgram( programID );
    for( usize i = 0; i < count; i++ ) {
        glDetachShader( programID, shaders[i] );
        glDeleteShader( shaders[i] );
    }

    i32 success;
    glGetProgramiv( programID, GL_LINK_STATUS, &success );
    if( success == GL_FALSE ) {
        i32 len = 0;
        glGetProgramiv( programID, GL_INFO_LOG_LENGTH, &len );
        char* buffer = (char*)malloc( len );
        glGetProgramInfoLog(
            programID,
            len,
            nullptr,
            buffer
        );
        LOG_ERROR("OpenGL Linking Error > %s", buffer);
        free(buffer);
        return false;
    } else { return true; }

}

bool Renderer::OpenGLAPI::GetUniform( RendererID shaderID, const std::string& uniformName, UniformID& uniform ) {
    uniform = glGetUniformLocation( shaderID, uniformName.c_str() );
    if(uniform < 0) {
        LOG_ERROR("OpenGL > Uniform \"%s\" could not be found!", uniformName.c_str());
        return false;
    } else {
        return true;
    }
}

#ifdef DEBUG

const usize IGNORE_IDS[] = { 131185 };
const usize IGNORE_IDS_COUNT = 1;

void OpenGLDebugMessageCallback(
    GLenum, // source
    GLenum, // type
    GLuint id,
    GLenum severity,
    GLsizei, // length
    const GLchar *message,
    const void* // userParam
) {
    for( usize i = 0; i < IGNORE_IDS_COUNT; i++ ) {
        if( id == IGNORE_IDS[i] ) {
            return;
        }
    }

    switch(severity) {
        case GL_DEBUG_SEVERITY_HIGH: {
            LOG_ERROR("OpenGL [%i] > %s", id, message);
        } break;
        case GL_DEBUG_SEVERITY_MEDIUM:
        case GL_DEBUG_SEVERITY_LOW: {
            LOG_WARN("OpenGL [%i] > %s", id, message);
        } break;
        default: case GL_DEBUG_SEVERITY_NOTIFICATION: {
            LOG_INFO("OpenGL [%i] > %s", id, message);
        } break;
    }
}
#endif

bool Renderer::OpenGLAPI::Initialize() {

#ifdef DEBUG
    glEnable( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( OpenGLDebugMessageCallback, nullptr );
#endif

    RendererID fontVert, fontFrag;
    auto fontVertSrc = IO::LoadFile( "./resources/shaders/font/font.glslVert" );
    if(fontVertSrc.contents) {
        bool compileResult = CompileShader(
            (const char*)fontVertSrc.contents,
            (i32)fontVertSrc.size,
            GL_VERTEX_SHADER,
            fontVert
        );
        IO::FreeFile(fontVertSrc);
        if(!compileResult) {
            return false;
        }
    } else {
        LOG_ERROR("OpenGL > Failed to load font vertex shader source from disk!");
        return false;
    }
    auto fontFragSrc = IO::LoadFile( "./resources/shaders/font/font.glslFrag" );
    if(fontFragSrc.contents) {
        bool compileResult = CompileShader(
            (const char*)fontFragSrc.contents,
            (i32)fontFragSrc.size,
            GL_FRAGMENT_SHADER,
            fontFrag
        );
        IO::FreeFile(fontFragSrc);
        if(!compileResult) {
            return false;
        }
    } else {
        LOG_ERROR("OpenGL > Failed to load font fragment shader source from disk!");
        return false;
    }

    RendererID fontShaders[] = { fontVert, fontFrag };
    if(!LinkShaders( fontShaders, 2, m_fontProgram )) {
        return false;
    }

    glUseProgram( m_fontProgram );

    GetUniform( m_fontProgram, "u_transform", m_textTransformID );

    UniformID textSamplerID;
    GetUniform( m_fontProgram, "u_texture", textSamplerID );
    glUniform1i( textSamplerID, 0 );

    GetUniform( m_fontProgram, "u_viewProjection", m_textProjectionID );
    GetUniform( m_fontProgram, "u_color", m_textColorID );
    GetUniform( m_fontProgram, "u_fontCoords", m_textFontCoordsID );

    glUniform4fv( m_textColorID, 1, glm::value_ptr(glm::vec4(1.0f)) );

    glm::mat4 textProjection = glm::ortho( -1.0f, 1.0f, -1.0f, 1.0f );
    glUniformMatrix4fv( m_textProjectionID, 1, GL_FALSE, glm::value_ptr(textProjection) );

    // f32 fontVertices[] = {
    //     /*POSITION*/ -0.5f,  0.5f, /*UV*/ 0.0f, 1.0f,
    //     /*POSITION*/  0.5f,  0.5f, /*UV*/ 1.0f, 1.0f,
    //     /*POSITION*/ -0.5f, -0.5f, /*UV*/ 0.0f, 0.0f,
    //     /*POSITION*/  0.5f, -0.5f, /*UV*/ 1.0f, 0.0f
    // };
    f32 fontVertices[] = {
        /*POSITION*/ 0.0f, 1.0f, /*UV*/ 0.0f, 1.0f,
        /*POSITION*/ 1.0f, 1.0f, /*UV*/ 1.0f, 1.0f,
        /*POSITION*/ 0.0f, 0.0f, /*UV*/ 0.0f, 0.0f,
        /*POSITION*/ 1.0f, 0.0f, /*UV*/ 1.0f, 0.0f
    };
    const usize FONT_VERTEX_COUNT = 16;
    u32 fontIndices[] = {
        0, 1, 2,
        1, 2, 3
    };
    const usize FONT_INDEX_COUNT = 6;

    glGenVertexArrays( 1, &m_fontVAO );
    glBindVertexArray( m_fontVAO );
    GLuint fontVBO, fontEBO;
    glGenBuffers(1, &fontVBO);
    glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(f32) * FONT_VERTEX_COUNT,
        &fontVertices,
        GL_STATIC_DRAW
    );
    glVertexAttribPointer(
        0, // index
        4, // size
        GL_FLOAT, // type
        GL_FALSE, // normalized
        sizeof(f32) * 4, // stride
        0 // pointer
    );
    glEnableVertexAttribArray(0);

    glGenBuffers( 1, &fontEBO );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, fontEBO );
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(u32) * FONT_INDEX_COUNT,
        &fontIndices,
        GL_STATIC_DRAW
    );

    return true;
}

bool Renderer::LoadOpenGLFunctions( OpenGLLoader loader ) {
    return gladLoadGLLoader( (GLADloadproc)loader ) != 0;
}
