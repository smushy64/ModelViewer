#include "renderer.hpp"
#include "gl.hpp"
#include "debug.hpp"
#include "global.hpp"
#include "consts.hpp"
#include "utils.hpp"
#include "core/ui.hpp"
#include "core/image.hpp"
#include "core/light.hpp"
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

    m_camera = new Core::Camera();
    m_lights = new Core::Lights();

    auto ortho = glm::ortho( 0.0f, 1.0f, 0.0f, 1.0f );
    m_matrices2D = UniformBuffer::New( sizeof( glm::mat4 ), glm::value_ptr(ortho) );
    m_matrices2D->SetBindingPoint( MATRIX_2D_BINDING_POINT );

    m_sharedData = UniformBuffer::New( SHARED_DATA_SIZE, nullptr );
    m_sharedData->SetBindingPoint( SHARED_DATA_BINDING_POINT );
    BufferCameraPosition( glm::vec3(0.0f) );
    BufferClippingFields( glm::vec2( 0.001f, 1000.0f ) );

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
    /* Create Font Shader */ {
        TextFile fontVertSrc = LoadTextFile( FONT_VERT_PATH );
        TextFile fontFragSrc = LoadTextFile( FONT_FRAG_PATH );
        if( fontVertSrc.size == 0 || fontFragSrc.size == 0 ) {
            LOG_ERROR("Renderer > Failed to load font shaders from disk!");
            m_success = false;
            return;
        }
        
        m_fontShader = Shader::New( fontVertSrc.contents, fontFragSrc.contents );

        if( !m_fontShader->CompilationSucceeded() ) {
            LOG_ERROR( "Renderer > Failed to create font shader!" );
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
    /* Create Bounds Mesh */ {
        f32 boundsVertices[] = {
            /*POSITION*/ 0.0f, 1.0f,
            /*POSITION*/ 1.0f, 1.0f,
            /*POSITION*/ 0.0f, 0.0f,
            /*POSITION*/ 1.0f, 0.0f
        };
        const usize BOUNDS_VERTEX_COUNT = 8;
        u32 boundsIndices[] = {
            0, 1, 2,
            1, 2, 3
        };
        const usize BOUNDS_INDEX_COUNT = 6;

        m_boundsVA = VertexArray::New();
        m_boundsVA->UseArray();

        auto boundsVB = VertexBuffer::New( sizeof(f32) * BOUNDS_VERTEX_COUNT, &boundsVertices );
        boundsVB->SetLayout(BufferLayout({
            NewBufferElement( "Vertices", BufferDataType::FLOAT, BufferDataStructure::VEC2, false )
        }));
        m_boundsVA->AddVertexBuffer( boundsVB );

        auto boundsIB = IndexBuffer::New( BufferDataType::UINT, BOUNDS_INDEX_COUNT, &boundsIndices );
        m_boundsVA->SetIndexBuffer( boundsIB );

    };
    /* Create Bounds Shader */ {
        TextFile vsrc = LoadTextFile( BOUNDS_VERT_PATH );
        TextFile fsrc = LoadTextFile( BOUNDS_FRAG_PATH );

        if( vsrc.size == 0 || fsrc.size == 0 ) {
            LOG_ERROR("Renderer > Failed to load bounds source!");
            m_success = false;
            return;
        }

        m_boundsShader = Shader::New( vsrc.contents, fsrc.contents );
        if( !m_boundsShader->CompilationSucceeded() ) {
            m_success = false;
            return;
        }

        m_boundsShader->UseShader();
        m_boundsShader->GetUniform( "u_transform", m_boundsTransformID );
    }
    /* Create Cube Mesh */ {
        m_meshVA = VertexArray::New();
        m_meshVA->UseArray();

        auto vertices = Utils::CreateCubeMesh();
        auto vBuffer = VertexBuffer::New( sizeof(Vertex) * vertices.size(), &vertices[0] );
        auto vLayout = StandardVertexLayout();
        vBuffer->SetLayout( vLayout );

        m_meshVA->AddVertexBuffer( vBuffer );

        auto indices = Utils::CubeIndices();
        auto iBuffer = IndexBuffer::New(
            BufferDataType::UINT,
            indices.size(),
            &indices[0]
        );

        m_meshVA->SetIndexBuffer( iBuffer );
    }
    /* Create Blinn-Phong Shader */ {
        Platform::TextFile vsrc = Platform::LoadTextFile( BLINNPHONG_VERT_PATH );
        Platform::TextFile fsrc = Platform::LoadTextFile( BLINNPHONG_FRAG_PATH );

        if( vsrc.size == 0 || fsrc.size == 0 ) {
            LOG_ERROR("Renderer > Failed to load blinn-phong source!");
            m_success = false;
            return;
        }

        auto shader = Shader::New( vsrc.contents, fsrc.contents );
        if( !shader->CompilationSucceeded() ) {
            m_success = false;
            return;
        }

        m_blinnPhong = new BlinnPhong( shader );
    }

    LOG_INFO("Renderer > Initialized Successfully");
}
void Renderer::BufferCameraPosition( const glm::vec3& cameraPosition ) {
    m_sharedData->BufferSubData( 0, sizeof( glm::vec3 ), glm::value_ptr( cameraPosition ) );
}
void Renderer::BufferClippingFields( const glm::vec2& clippingFields ) {
    m_sharedData->BufferSubData( sizeof( glm::vec4 ), sizeof( glm::vec2 ), glm::value_ptr( clippingFields ) );
}
void Renderer::RenderText( const Core::UI::Label& label ) const {
    RenderText(
        label.Text(),
        label.ScreenSpacePosition(),
        label.Scale(),
        label.Color(),
        label.AnchorX(),
        label.AnchorY(),
        label.Font()
    );
}
void Renderer::RenderTextButton( const Core::UI::LabelButton& labelButton ) const {
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
        labelButton.AnchorY(),
        labelButton.Font()
    );
    if( m_renderBoundingBox ) {
        RenderBoundingBox( glm::vec4(
            (labelButton.ScreenSpacePosition().x * m_api->GetViewport().x) + labelButton.BoundingBoxOffset().x,
            (labelButton.ScreenSpacePosition().y * m_api->GetViewport().y) + labelButton.BoundingBoxOffset().y,
            labelButton.BoundingBox().x,
            labelButton.BoundingBox().y
        ) );
    }
}
void Renderer::SetMesh( VertexArray* va ) {
    if( va == nullptr ) {
        LOG_ERROR("Renderer > Attempted to set model ptr to a null ptr!");
        return;
    }
    delete( m_meshVA );
    m_meshVA = va;
}
void Renderer::RenderText(
    const std::string& text,
    const glm::vec2&   screenSpacePosition,
    f32                scale,
    const glm::vec4&   color,
    Core::XAnchor      anchorX,
    Core::YAnchor      anchorY
) const {
    RenderText(
        text,
        screenSpacePosition,
        scale,
        color,
        anchorX,
        anchorY,
        m_currentFont
    );
}
void Renderer::RenderText(
    const std::string& text,
    const glm::vec2&   screenSpacePosition,
    f32                scale,
    const glm::vec4&   color,
    Core::XAnchor      anchorX,
    Core::YAnchor      anchorY,
    const Core::FontAtlas* font
) const {
    m_api->EnableBlending();
    m_api->BlendFunction(
        RendererAPI::BlendFactor::SRC_ALPHA,
        RendererAPI::BlendFactor::ONE_MINUS_SRC_ALPHA
    );

    m_fontShader->UseShader();
    m_fontShader->UniformVec4( m_fontColorID, color );
    m_api->SetActiveTexture( 0 );
    font->texture->UseTexture();
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
                if( font->characterMetrics.count(*iter) == 0 ) {
                    // skip characters not found in character map
                    LOG_WARN("Renderer > Character \'%c\' not found in font \"%s\"",
                        *iter, font->name.c_str()
                    );
                    continue;
                }
                auto character = font->characterMetrics.at( *iter );
                stringWidth += character.advance * scale;
            }
            originX -= stringWidth / 2.0f;
        } break;
        case Core::XAnchor::RIGHT: {
            std::string::const_iterator iter;
            f32 stringWidth = 0.0f;
            for( iter = text.begin(); iter < text.end(); iter++ ) {
                if( font->characterMetrics.count(*iter) == 0 ) {
                    // skip characters not found in character map
                    LOG_WARN("Renderer > Character \'%c\' not found in font \"%s\"",
                        *iter, font->name.c_str()
                    );
                    continue;
                }
                auto character = font->characterMetrics.at( *iter );
                stringWidth += character.advance * scale;
            }
            originX -= stringWidth;
        } break;
        default: break;
    }

    switch(anchorY) {
        case Core::YAnchor::CENTER: {
            yOffset = -((font->pointSize / 2.0f) * scale);
        } break;
        case Core::YAnchor::TOP: {
            yOffset = -(font->pointSize * scale);
        } break;
        default: break;
    }

    std::string::const_iterator iter;
    for( iter = text.begin(); iter < text.end(); iter++ ) {
        if( font->characterMetrics.count(*iter) == 0 ) {
            // skip characters not found in character map
            LOG_WARN("Renderer > Character \'%c\' not found in font \"%s\"",
                *iter, font->name.c_str()
            );
            continue;
        }
        auto character = font->characterMetrics.at( *iter );
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
void Renderer::RenderCanvas( const Core::UI::Canvas& canvas ) const {
    for( auto const& label : canvas.GetLabels() ) {
        RenderText( label );
    }
    for( auto const& labelButton : canvas.GetLabelButtons() ) {
        RenderTextButton( labelButton );
    }
}
void Renderer::RenderCharacter(
    const Core::CharMetrics& metrics,
    const glm::vec2& origin,
    const glm::vec2& pixelPosition,
    f32 scale
) const {
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
    m_camera->SetAspectRatio( newResolution.x / newResolution.y );
}
void Renderer::SetCurrentFont( const Core::FontAtlas* font ) {
    if( font->texture == nullptr ) {
        LOG_ERROR( "Renderer > Attempted to set current font to a font that hasn't been uploaded to the GPU!" );
        return;
    }
    m_currentFont = font;
}
void Renderer::StartScene() {
    m_api->ClearBuffer();
    m_camera->RecalculateView();
    m_camera->RecalculateProjection();
    m_camera->RecalculateBasisVectors();
    BufferCameraPosition( m_camera->GetCameraPoint() );
    BufferClippingFields( m_camera->GetClippingFields() );
}
void Renderer::DrawMesh( Material material ) {
    switch( material ) {
        case Material::BLINNPHONG: {
            m_blinnPhong->UseMaterial();
            m_meshVA->UseArray();
            m_api->DrawVertexArray( m_meshVA );
        } break;
        default: {
            LOG_ERROR("Renderer > Attempted to Draw Mesh with unknown material!");
        } break;
    }
}
void Renderer::EndScene() {
    m_api->SwapBuffers();
}
Renderer::~Renderer() {
    delete( m_meshVA );
    delete( m_camera );
    delete( m_lights );
    delete( m_boundsShader );
    delete( m_boundsVA );
    delete( m_blinnPhong );
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

void Renderer::RenderBoundingBox( const glm::vec4& bounds ) const {
    glm::mat4 transform = glm::scale(
        glm::translate( glm::mat4(1.0f), glm::vec3(
            bounds.x,
            bounds.y,
            0.0f
        ) ),
        glm::vec3( bounds.z, bounds.w, 0.0f )
    );

    m_boundsShader->UseShader();
    m_boundsShader->UniformMat4( m_boundsTransformID, transform );
    
    m_api->EnableWireframe();

    m_boundsVA->UseArray();
    m_api->DrawVertexArray( m_boundsVA );

    m_api->DisableWireframe();
}

BlinnPhong::BlinnPhong( Shader* shader )
: m_shader(shader) {
    m_shader->GetUniform( "u_transform", m_transformID );
    m_shader->GetUniform( "u_normalMat", m_normalMatID );
    m_shader->GetUniform( "u_surfaceTint", m_tintID );
    m_shader->GetUniform( "u_glossiness", m_glossinessID );
    m_shader->GetUniform( "u_albedoPresent", m_albedoPresentID );
    m_shader->GetUniform( "u_specularPresent", m_specularPresentID );
    m_shader->GetUniform( "u_normalPresent", m_normalPresentID );
}
BlinnPhong::~BlinnPhong() {
    delete( m_shader );
    if( m_albedoTexture != nullptr ) {
        delete( m_albedoTexture );
    }
    if( m_specularTexture != nullptr ) {
        delete( m_specularTexture );
    }
    if( m_normalTexture != nullptr ) {
        delete( m_normalTexture );
    }
}
void BlinnPhong::SetPosition( const glm::vec3& position ) {
    m_position = position;
    m_transformDirty = true;
}
void BlinnPhong::SetRotation( const glm::fquat& rotation ) {
    m_rotation = rotation;
    m_transformDirty = true;
}
void BlinnPhong::SetScale( const glm::vec3& scale ) {
    m_scale = scale;
    m_transformDirty = true;
}
void BlinnPhong::SetTint( const glm::vec3& tint ) {
    m_tint = tint;
    m_tintDirty = true;
}
void BlinnPhong::SetGlossiness( f32 glossiness ) {
    m_glossiness = glossiness;
    m_glossinessDirty = true;
}
void BlinnPhong::UseMaterial() {
    if( m_transformDirty ) {
        auto position = glm::translate( glm::mat4(1.0f), m_position );
        auto rotation = glm::toMat4( m_rotation );
        auto scale = glm::scale( glm::mat4( 1.0f ), m_scale );
        m_transform = position * rotation * scale;
        m_normalMat = glm::transpose( glm::inverse( m_transform ) );
        m_transformDirty = false;
        m_shader->UniformMat4( m_transformID, m_transform );
        m_shader->UniformMat3( m_normalMatID, m_normalMat );
    }
    if( m_tintDirty ) {
        m_shader->UniformVec3( m_tintID, m_tint );
        m_tintDirty = false;
    }
    if( m_glossinessDirty ) {
        m_shader->UniformFloat( m_glossinessID, m_glossiness );
        m_glossinessDirty = false;
    }

    m_shader->UseShader();
    if( m_albedoTexture == nullptr ) {
        m_shader->UniformInt( m_albedoPresentID, 0 );
    } else {
        m_shader->UniformInt( m_albedoPresentID, 1 );
        m_albedoTexture->UseTexture( ALBEDO_SAMPLER );
    }

    if( m_specularTexture == nullptr ) {
        m_shader->UniformInt( m_specularPresentID, 0 );
    } else {
        m_shader->UniformInt( m_specularPresentID, 1 );
        m_specularTexture->UseTexture( SPECULAR_SAMPLER );
    }

    if( m_normalTexture == nullptr ) {
        m_shader->UniformInt( m_normalPresentID, 0 );
    } else {
        m_shader->UniformInt( m_normalPresentID, 1 );
        m_normalTexture->UseTexture( NORMAL_SAMPLER );
    }
}
void BlinnPhong::SetAlbedo( const Texture2D* albedo ) {
    if( m_albedoTexture != nullptr ) {
        delete( m_albedoTexture );
    }
    m_albedoTexture = albedo;
}
void BlinnPhong::SetSpecular( const Texture2D* specular ) {
    if( m_specularTexture != nullptr ) {
        delete( m_specularTexture );
    }
    m_specularTexture = specular;
}
void BlinnPhong::SetNormal( const Texture2D* normal ) {
    if( m_normalTexture != nullptr ) {
        delete( m_normalTexture );
    }
    m_normalTexture = normal;
}

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
        usize size = BufferDataStructureCount(element.dataStructure) * BufferDataTypeByteSize(element.dataType);
        offset   += size;
        m_stride += size;
    }
}

Platform::BufferLayout Platform::StandardVertexLayout() {
    return Platform::BufferLayout({
        Platform::NewBufferElement(
            "Position",
            Platform::BufferDataType::FLOAT,
            Platform::BufferDataStructure::VEC3,
            false
        ),
        Platform::NewBufferElement(
            "UV",
            Platform::BufferDataType::FLOAT,
            Platform::BufferDataStructure::VEC2,
            false
        ),
        Platform::NewBufferElement(
            "Normal",
            Platform::BufferDataType::FLOAT,
            Platform::BufferDataStructure::VEC3,
            false
        ),
        Platform::NewBufferElement(
            "Tangent",
            Platform::BufferDataType::FLOAT,
            Platform::BufferDataStructure::VEC3,
            false
        ),
        Platform::NewBufferElement(
            "Bitangent",
            Platform::BufferDataType::FLOAT,
            Platform::BufferDataStructure::VEC3,
            false
        )
    });
}

Platform::TextureFormat Platform::ImageFormatToTextureFormat( Core::ImageFormat imageFormat ) {
    switch(imageFormat) {
        case Core::ImageFormat::R: return TextureFormat::R;
        case Core::ImageFormat::RG: return TextureFormat::RG;
        case Core::ImageFormat::RGB: return TextureFormat::RGB;
        case Core::ImageFormat::RGBA: return TextureFormat::RGBA;
        default: {
            LOG_ERROR("Renderer > Image Format does not have an equivalent texture format!");
            return (TextureFormat)-1;
        }
    }
}
Platform::TextureInternalFormat Platform::ImageFormatToTextureInternalFormat( Core::ImageFormat imageFormat ) {
    switch(imageFormat) {
        case Core::ImageFormat::R: return TextureInternalFormat::R;
        case Core::ImageFormat::RG: return TextureInternalFormat::RG;
        case Core::ImageFormat::RGB: return TextureInternalFormat::RGB;
        case Core::ImageFormat::RGBA: return TextureInternalFormat::RGBA;
        default: {
            LOG_ERROR("Renderer > Image Format does not have an equivalent texture format!");
            return (TextureInternalFormat)-1;
        }
    }
}
