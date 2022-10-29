#include "renderer.hpp"
#include "gl.hpp"
#include "debug.hpp"
#include "global.hpp"
#include "consts.hpp"
#include <glm/gtc/type_ptr.hpp>

using namespace Platform;

const char* FONT_VERT_PATH = "./resources/shaders/font/font.glslVert";
const char* FONT_FRAG_PATH = "./resources/shaders/font/font.glslFrag";

const char* BOUNDS_VERT_PATH = "./resources/shaders/bounds/bounds.glslVert";
const char* BOUNDS_FRAG_PATH = "./resources/shaders/bounds/bounds.glslFrag";

const char* BLINNPHONG_VERT_PATH = "./resources/shaders/blinn_phong/blinn_phong.glslVert";
const char* BLINNPHONG_FRAG_PATH = "./resources/shaders/blinn_phong/blinn_phong.glslFrag";

const char* Platform::BackendToString( BackendAPI backend ) {
    switch( backend ) {
        case BackendAPI::OPENGL: return "OpenGL Core 4.6";
        default: return "UNKNOWN";
    }
}
const char* RendererAPI::BlendFactorToString( BlendFactor blendFactor ) {
    switch( blendFactor ) {
        case BlendFactor::ZERO:                return "ZERO";
        case BlendFactor::ONE:                 return "ONE";
        case BlendFactor::SRC_COLOR:           return "SRC_COLOR";
        case BlendFactor::ONE_MINUS_SRC_COLOR: return "ONE_MINUS_SRC_COLOR";
        case BlendFactor::DST_COLOR:           return "DST_COLOR";
        case BlendFactor::ONE_MINUS_DST_COLOR: return "ONE_MINUS_DST_COLOR";
        case BlendFactor::SRC_ALPHA:           return "SRC_ALPHA";
        case BlendFactor::ONE_MINUS_SRC_ALPHA: return "ONE_MINUS_SRC_ALPHA";
        case BlendFactor::DST_ALPHA:           return "DST_ALPHA";
        case BlendFactor::ONE_MINUS_DST_ALPHA: return "ONE_MINUS_DST_ALPHA";
        case BlendFactor::CONSTANT_COLOR:      return "CONSTANT_COLOR";
        case BlendFactor::ONE_MINUS_CONSTANT_COLOR: return "ONE_MINUS_CONSTANT_COLOR";
        case BlendFactor::CONSTANT_ALPHA:      return "CONSTANT_ALPHA";
        case BlendFactor::ONE_MINUS_CONSTANT_ALPHA: return "ONE_MINUS_CONSTANT_ALPHA";
        case BlendFactor::SRC_ALPHA_SATURATE:   return "SRC_ALPHA_SATURATE";
        case BlendFactor::SRC1_COLOR:           return "SRC1_COLOR";
        case BlendFactor::ONE_MINUS_SRC1_COLOR: return "ONE_MINUS_SRC1_COLOR";
        case BlendFactor::SRC1_ALPHA:           return "SRC1_ALPHA";
        case BlendFactor::ONE_MINUS_SRC1_ALPHA: return "ONE_MINUS_SRC1_ALPHA";
        default: return "UNKNOWN";
    }
}
const char* RendererAPI::BlendEqToString( BlendEq eq ) {
    switch( eq ) {
        case BlendEq::ADD:     return "ADD";
        case BlendEq::SUB:     return "SUB";
        case BlendEq::REV_SUB: return "REV_SUB";
        case BlendEq::MIN:     return "MIN";
        case BlendEq::MAX:     return "MAX";
        default: return "UNKNOWN";
    }
}
const char* RendererAPI::PixelAlignmentToString( PixelAlignment alignment ) {
    switch(alignment) {
        case PixelAlignment::TWO  : return "Pixel Alignment 2";
        case PixelAlignment::FOUR : return "Pixel Alignment 4";
        case PixelAlignment::EIGHT: return "Pixel Alignment 8";
        default: return "Pixel Alignment 1";
    }
}
const char* Platform::TextureFormatToString( TextureFormat format ) {
    switch( format ) {
        case TextureFormat::DEPTH: return "DEPTH";
        case TextureFormat::DEPTH_STENCIL: return "DEPTH_STENCIL";
        case TextureFormat::R: return "RED";
        case TextureFormat::RG: return "RED/GREEN";
        case TextureFormat::RGB: return "RED/GREEN/BLUE";
        case TextureFormat::RGBA: return "RED/GREEN/BLUE/ALPHA";
        default: return "UNKNOWN";
    }
}
const char* Platform::TextureInternalFormatToString( TextureInternalFormat format ) {
    switch( format ) {
        case TextureInternalFormat::DEPTH: return "DEPTH";
        case TextureInternalFormat::DEPTH_STENCIL: return "DEPTH_STENCIL";
        case TextureInternalFormat::R: return "RED";
        case TextureInternalFormat::RG: return "RED/GREEN";
        case TextureInternalFormat::RGB: return "RED/GREEN/BLUE";
        case TextureInternalFormat::RGBA: return "RED/GREEN/BLUE/ALPHA";
        default: return "UNKNOWN";
    }
}
const char* Platform::TextureWrapModeToString( TextureWrapMode wrap ) {
    switch( wrap ) {
        case TextureWrapMode::CLAMP: return "CLAMP";
        case TextureWrapMode::REPEAT: return "REPEAT";
        case TextureWrapMode::MIRROR_REPEAT: return "MIRROR_REPEAT";
        case TextureWrapMode::MIRROR_CLAMP: return "MIRROR_CLAMP";
        default: return "UNKNOWN";
    }
}
const char* Platform::TextureFilterMinToString( TextureFilterMin filter ) {
    switch( filter ) {
        case TextureFilterMin::NEAREST: return "NEAREST";
        case TextureFilterMin::LINEAR: return "LINEAR";
        case TextureFilterMin::NEAREST_MM_NEAREST: return "NEAREST_MM_NEAREST";
        case TextureFilterMin::LINEAR_MM_NEAREST: return "LINEAR_MM_NEAREST";
        case TextureFilterMin::NEAREST_MM_LINEAR: return "NEAREST_MM_LINEAR";
        case TextureFilterMin::LINEAR_MM_LINEAR: return "LINEAR_MM_LINEAR";
        default: return "UNKNOWN";
    }
}
const char* Platform::TextureFilterMagToString( TextureFilterMag filter ) {
    switch( filter ) {
        case TextureFilterMag::NEAREST: return "NEAREST";
        case TextureFilterMag::LINEAR: return "LINEAR";
        default: return "UNKNOWN";
    }
}

Renderer::Renderer( BackendAPI backend ) {
    m_success = true;
    LOG_INFO("Renderer > Current Backend: %s", BackendToString(backend));
    switch( backend ) {
        case BackendAPI::OPENGL: {
            m_api = new RendererAPIOpenGL();
        } break;
        default: {
            LOG_ERROR("Renderer > Backend not yet implemented!");
            m_success = false;
            return;
        }
    }
    CURRENT_BACKEND = backend;
}
void Renderer::Initialize() {
    m_api->Initialize();
    LOG_INFO("Renderer > API Initialized");

    auto ortho = glm::ortho( 0.0f, (f32)DEFAULT_WINDOW_WIDTH, 0.0f, (f32)DEFAULT_WINDOW_HEIGHT );
    m_matrices2D = UniformBuffer::New( sizeof( glm::mat4 ), glm::value_ptr(ortho) );
    m_matrices2D->SetBindingPoint( MATRIX_2D_BINDING_POINT );

    m_sharedData = UniformBuffer::New( SHARED_DATA_SIZE, nullptr );
    m_sharedData->SetBindingPoint( SHARED_DATA_BINDING_POINT );
    BufferCameraPosition( glm::vec3(0.0f) );
    BufferClippingFields( glm::vec2( 0.001f, 1000.0f ) );

    LOG_INFO("Renderer > Buffers Created");

    /* Create Font Mesh */ {
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

        m_fontVA = VertexArray::New();
        m_fontVA->UseArray();

        VertexBuffer* fontVertexBuffer = VertexBuffer::New( sizeof(f32) * FONT_VERTEX_COUNT, &fontVertices );

        fontVertexBuffer->SetLayout(BufferLayout({
            NewBufferElement( "Vertices", BufferDataType::FLOAT, BufferDataStructure::VEC4, false )
        }));

        m_fontVA->AddVertexBuffer( fontVertexBuffer );
        IndexBuffer* fontIndexBuffer = IndexBuffer::New( BufferDataType::UINT, FONT_INDEX_COUNT, &fontIndices );
        m_fontVA->SetIndexBuffer(fontIndexBuffer);
    }

    LOG_INFO("Renderer > Font Mesh Created");

    /* Create Font Shader */ {
        TextFile fontVertSrc = LoadTextFile( FONT_VERT_PATH );
        TextFile fontFragSrc = LoadTextFile( FONT_FRAG_PATH );
        if( fontVertSrc.size == 0 || fontFragSrc.size == 0 ) {
            LOG_ERROR("OpenGL > Failed to load font shaders from disk!");
            m_success = false;
            return;
        }
        
        m_fontShader = Shader::New( fontVertSrc.contents, fontFragSrc.contents );

        if( !m_fontShader->CompilationSucceeded() ) {
            LOG_ERROR( "OpenGL > Failed to create font shader!" );
            m_success = false;
            return;
        }

        m_fontShader->UseShader();
        m_fontShader->GetUniform( "u_transform", m_fontTransformID );
        m_fontShader->GetUniform( "u_color", m_fontColorID );
        m_fontShader->GetUniform( "u_fontCoords", m_fontCoordsID );
        UniformID texSamplerID;
        m_fontShader->GetUniform( "u_texture", texSamplerID );

        m_fontShader->UniformInt( texSamplerID, 0 );
        m_fontShader->UniformVec4( m_fontColorID, glm::vec4(1.0f) );
    }

    LOG_INFO("Renderer > Font Shader Created");
}
void Renderer::BufferCameraPosition( const glm::vec3& cameraPosition ) {
    m_sharedData->BufferSubData( 0, sizeof( glm::vec3 ), glm::value_ptr( cameraPosition ) );
}
void Renderer::BufferClippingFields( const glm::vec2& clippingFields ) {
    m_sharedData->BufferSubData( sizeof( glm::vec4 ), sizeof( glm::vec2 ), glm::value_ptr( clippingFields ) );
}
void Renderer::RenderText( const Core::UI::Label& label ) {
    SetCurrentFont( label.Font() );
    RenderText(
        label.Text(),
        label.ScreenSpacePosition(),
        label.Scale(),
        label.Color(),
        label.AnchorX(),
        label.AnchorY()
    );
}
void Renderer::RenderTextButton( const Core::UI::LabelButton& labelButton ) {
    SetCurrentFont( labelButton.Font() );
    glm::vec4 color;
    switch( labelButton.State() ) {
        case Core::UI::ElementState::HOVERED: {
            color = labelButton.HoverColor();
        } break;
        case Core::UI::ElementState::PRESSED: {
            color = labelButton.PressColor();
        } break;
        default: color = labelButton.BaseColor();
    }
    RenderText(
        labelButton.Text(),
        labelButton.ScreenSpacePosition(),
        labelButton.Scale(),
        color,
        labelButton.AnchorX(),
        labelButton.AnchorY()
    );
}
void Renderer::RenderText(
    const std::string& text,
    const glm::vec2&   screenSpacePosition,
    f32                scale,
    const glm::vec4&   color,
    Core::XAnchor      anchorX,
    Core::YAnchor      anchorY
) {
    if( m_currentFont == nullptr ) {
        LOG_ERROR("Renderer > Attempted to render text while there is no font set!");
        return;
    }
    
    m_api->EnableBlending();
    m_api->BlendFunction(
        RendererAPI::BlendFactor::SRC_ALPHA,
        RendererAPI::BlendFactor::ONE_MINUS_SRC_ALPHA
    );

    m_fontShader->UseShader();
    m_fontShader->UniformVec4( m_fontColorID, color );
    m_api->SetActiveTexture( 0 );
    m_currentFont->texture->UseTexture();
    m_fontVA->UseArray();

    glm::vec2 pixelPosition = glm::vec2(
        screenSpacePosition.x * m_api->GetViewport().x,
        screenSpacePosition.y * m_api->GetViewport().y
    );
    
    f32 originX = pixelPosition.x;
    f32 yOffset = 0.0f;
    switch(anchorX) {
        case Core::XAnchor::CENTER: {
            std::string::const_iterator iter;
            f32 stringWidth = 0.0f;
            for( iter = text.begin(); iter < text.end(); iter++ ) {
                if( m_currentFont->characterMetrics.count(*iter) == 0 ) {
                    // skip characters not found in character map
                    LOG_WARN("Renderer > Character \'%c\' not found in font \"%s\"",
                        *iter, m_currentFont->name.c_str()
                    );
                    continue;
                }
                auto character = m_currentFont->characterMetrics.at( *iter );
                stringWidth += character.advance * scale;
            }
            originX -= stringWidth / 2.0f;
        } break;
        case Core::XAnchor::RIGHT: {
            std::string::const_iterator iter;
            f32 stringWidth = 0.0f;
            for( iter = text.begin(); iter < text.end(); iter++ ) {
                if( m_currentFont->characterMetrics.count(*iter) == 0 ) {
                    // skip characters not found in character map
                    LOG_WARN("Renderer > Character \'%c\' not found in font \"%s\"",
                        *iter, m_currentFont->name.c_str()
                    );
                    continue;
                }
                auto character = m_currentFont->characterMetrics.at( *iter );
                stringWidth += character.advance * scale;
            }
            originX -= stringWidth;
        } break;
        default: break;
    }

    switch(anchorY) {
        case Core::YAnchor::CENTER: {
            yOffset = -((m_currentFont->pointSize / 2.0f) * scale);
        } break;
        case Core::YAnchor::TOP: {
            yOffset = -(m_currentFont->pointSize * scale);
        } break;
        default: break;
    }

    std::string::const_iterator iter;
    for( iter = text.begin(); iter < text.end(); iter++ ) {
        if( m_currentFont->characterMetrics.count(*iter) == 0 ) {
            // skip characters not found in character map
            LOG_WARN("Renderer > Character \'%c\' not found in font \"%s\"",
                *iter, m_currentFont->name.c_str()
            );
            continue;
        }
        auto character = m_currentFont->characterMetrics.at( *iter );
        RenderCharacter(
            character,
            glm::vec2(originX, yOffset),
            pixelPosition,
            scale
        );
        originX += character.advance * scale;
    }

    m_api->DisableBlending();

}
void Renderer::RenderCharacter(
    const Core::CharMetrics& metrics,
    const glm::vec2& origin,
    const glm::vec2& pixelPosition,
    f32 scale
) {
    glm::vec3 characterScale = glm::vec3( metrics.width, metrics.height, 0.0f ) * scale;
    glm::vec3 characterTranslate = glm::vec3(
        origin.x + (metrics.leftBearing * scale),
        (pixelPosition.y + origin.y) - (metrics.topBearing * scale),
        0.0f
    );

    glm::mat4 transform = glm::scale(
        glm::translate( glm::mat4(1.0f), characterTranslate ),
        characterScale
    );
    m_fontShader->UniformMat4( m_fontTransformID, transform );

    glm::vec4 fontCoords = glm::vec4(
        metrics.atlasX,     // atlas x
        metrics.atlasY,     // atlas y
        metrics.atlasWidth, // width
        metrics.atlasHeight // height
    );

    m_fontShader->UniformVec4( m_fontCoordsID, fontCoords );
    m_api->DrawVertexArray( m_fontVA );
}
void Renderer::ResolutionChanged( const glm::vec2& newResolution ) {
    m_api->SetViewport( newResolution );
    auto newOrtho = glm::ortho( 0.0f, newResolution.x, 0.0f, newResolution.y );
    m_matrices2D->BufferData( sizeof(glm::mat4), glm::value_ptr(newOrtho) );
}
void Renderer::SetCurrentFont( const Core::FontAtlas* font ) {
    if( font->texture == nullptr ) {
        LOG_ERROR( "Renderer > Attempted to set current font to a font that hasn't been uploaded to the GPU!" );
        return;
    }
    m_currentFont = font;
}
Renderer::~Renderer() {
    if( m_modelVA != nullptr ) {
        delete( m_modelVA );
    }
    delete( m_modelShader );
    delete( m_matrices2D );
    delete( m_fontShader );
    delete( m_fontVA );
    delete( m_api );
}

void Renderer::UploadFontAtlasBitmap( Core::FontAtlas& fontAtlas ) {
    m_api->SetPackAlignment( RendererAPI::PixelAlignment::ONE );

    if( fontAtlas.bitmap == nullptr ) {
        LOG_ERROR("Renderer > Font Atlas Bitmap is null!");
        return;
    }

    fontAtlas.texture = Texture2D::New(
        glm::ivec2(fontAtlas.atlasScale,fontAtlas.atlasScale),
        (void*)fontAtlas.bitmap,
        TextureFormat::R,
        TextureInternalFormat::R,
        BufferDataType::UBYTE,
        AUTO_MIPMAP,
        false
    );
    fontAtlas.texture->UseTexture();
    fontAtlas.texture->SetHorizontalWrap( TextureWrapMode::CLAMP );
    fontAtlas.texture->SetVerticalWrap( TextureWrapMode::CLAMP );
    fontAtlas.texture->SetMinificationFilter( TextureFilterMin::LINEAR );
    fontAtlas.texture->SetMagnificationFilter( TextureFilterMag::LINEAR );
    
    free( fontAtlas.bitmap );
    
    m_api->SetPackAlignment( RendererAPI::PixelAlignment::FOUR );
}

// #ifdef DEBUG
//     RenderBoundingBox( glm::vec4(
//         (button.ScreenSpacePosition().x * m_viewport.x) + button.BoundingBoxOffset().x,
//         (button.ScreenSpacePosition().y * m_viewport.y) + button.BoundingBoxOffset().y,
//         button.BoundingBox().x,
//         button.BoundingBox().y
//     ) );
// #endif
// }

Platform::Shader* Platform::Shader::New( const std::string& vertex, const std::string& fragment ) {
    switch( CURRENT_BACKEND ) {
        case BackendAPI::OPENGL:
            return new ShaderOpenGL( vertex, fragment );
        default:{
            LOG_ERROR("Current Backend does not yet implement shaders!");
        }return nullptr;
    }
}

Platform::UniformBuffer* Platform::UniformBuffer::New( usize size, const void* data ) {
    switch( CURRENT_BACKEND ) {
        case BackendAPI::OPENGL:
            return new UniformBufferOpenGL( size, data );
        default:{
            LOG_ERROR("Current Backend does not yet implement buffers!");
        }return nullptr;
    }
}

VertexArray* VertexArray::New() {
    switch( CURRENT_BACKEND ) {
        case BackendAPI::OPENGL:
            return new VertexArrayOpenGL();
        default:{
            LOG_ERROR("Current Backend does not yet implement vertex arrays!");
        }return nullptr;
    }
}

IndexBuffer* IndexBuffer::New( BufferDataType type, usize count, const void* data ) {
    switch( CURRENT_BACKEND ) {
        case BackendAPI::OPENGL:
            return new IndexBufferOpenGL( type, count, data );
        default:{
            LOG_ERROR("Current Backend does not yet implement buffers!");
        }return nullptr;
    }
}

VertexBuffer* VertexBuffer::New( usize size, const void* data ) {
    switch( CURRENT_BACKEND ) {
        case BackendAPI::OPENGL:
            return new VertexBufferOpenGL( size, data );
        default:{
            LOG_ERROR("Current Backend does not yet implement buffers!");
        }return nullptr;
    }
}

Texture2D* Texture2D::New(
    const glm::ivec2& dimensions,
    const void* data,
    TextureFormat format,
    TextureInternalFormat internalFormat,
    BufferDataType dataType,
    i32 mipmapLevel,
    bool storeData
) {
    switch( CURRENT_BACKEND ) {
        case BackendAPI::OPENGL:
            return new Texture2DOpenGL(
                dimensions,
                data,
                format,
                internalFormat,
                dataType,
                mipmapLevel,
                storeData
            );
        default:{
            LOG_ERROR("Current Backend does not yet implement textures!");
        }return nullptr;
    }
}

usize Platform::BufferDataTypeByteSize( BufferDataType dataType ) {
    switch( dataType ) {
        case BufferDataType::BYTE:
        case BufferDataType::UBYTE:  return 1;

        case BufferDataType::USHORT:
        case BufferDataType::SHORT:  return 2;

        case BufferDataType::UINT:
        case BufferDataType::INT: 
        case BufferDataType::FLOAT:  return 4;
        case BufferDataType::BOOL:   return 4;

        case BufferDataType::DOUBLE: return 8;

        default: return 0;
    }
}

const char* Platform::BufferDataTypeToString( BufferDataType dataType ) {
    switch( dataType ) {
        case BufferDataType::FLOAT: return "FLOAT";
        case BufferDataType::DOUBLE: return "DOUBLE";
        case BufferDataType::UBYTE: return "UBYTE";
        case BufferDataType::USHORT: return "USHORT";
        case BufferDataType::UINT: return "UINT";
        case BufferDataType::BYTE: return "BYTE";
        case BufferDataType::SHORT: return "SHORT";
        case BufferDataType::INT: return "INT";
        case BufferDataType::BOOL: return "BOOL";
        default: return "UNKNOWN";
    }
}

usize Platform::BufferDataStructureCount( BufferDataStructure structure ) {
    switch( structure ) {
        case BufferDataStructure::SCALAR: return 1;
        case BufferDataStructure::VEC2:   return 2;
        case BufferDataStructure::VEC3:   return 3;
        case BufferDataStructure::VEC4:   return 4;
        case BufferDataStructure::MAT2:   return 4;
        case BufferDataStructure::MAT3:   return 9;
        case BufferDataStructure::MAT4:   return 16;

        default: return 0;
    }
}

const char* Platform::BufferDataStructureToString( BufferDataStructure structure ) {
    switch( structure ) {
        case BufferDataStructure::SCALAR: return "SCALAR";
        case BufferDataStructure::VEC2: return "VEC2";
        case BufferDataStructure::VEC3: return "VEC3";
        case BufferDataStructure::VEC4: return "VEC4";
        case BufferDataStructure::MAT2: return "MAT2";
        case BufferDataStructure::MAT3: return "MAT3";
        case BufferDataStructure::MAT4: return "MAT4";

        default: return "UNKNOWN";
    }
}

Platform::BufferElement Platform::NewBufferElement(
    const std::string&  name,
    BufferDataType      dataType,
    BufferDataStructure dataStructure,
    bool                normalized
) {
    BufferElement result = {};
    result.name          = name;
    result.dataType      = dataType;
    result.dataStructure = dataStructure;
    result.normalized    = normalized;
    return result;
}

void Platform::BufferLayout::CalculateOffsetsAndStride() {
    usize offset = 0;
    m_stride = 0;
    for( auto& element: m_elements ) {
        element.offset = offset;
        usize size = BufferDataStructureCount(element.dataStructure) * BufferDataTypeByteSize( element.dataType );
        offset   += size;
        m_stride += size;
    }
}
