/**
 * Description:  Application Definition
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 14, 2022 
*/
#include "app.hpp"
#include "util.hpp"

using Platform::KeyCode;

enum class TextAlignment {
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

void LoadMesh( Core::AppContext* app );
void LoadAlbedo( Core::AppContext* app );
void LoadNormal( Core::AppContext* app );
void LoadSpecular( Core::AppContext* app );
bool InitializeRenderContext( Core::AppContext* app );
void RenderText(
    const char* text,
    smath::vec2* screenPos,
    f32 scale,
    smath::vec4* color,
    TextAlignment alignment,
    Core::FontAtlas* fontAtlas,
    Core::AppContext* app
);
void Render( Core::AppContext* app );

void Core::OnUpdate( AppContext* app ) {
    static bool rendered = false;
    if( Platform::IsAppActive() ) {
        
        if( app->input.IsKeyDown( KeyCode::CTRL ) ) {
            if( app->input.IsKeyDown( KeyCode::M ) ) {
                LoadMesh( app );
            } else if( app->input.IsKeyDown( KeyCode::A ) ) {
                LoadAlbedo( app );
            } else if( app->input.IsKeyDown( KeyCode::G ) ) {
                LoadSpecular( app );
            } else if( app->input.IsKeyDown( KeyCode::N ) ) {
                LoadNormal( app );
            }
        }

        Platform::ResetCursorStyle();
        if(!rendered) {
            Render(app);
            rendered = true;
        }
    }
}

bool Core::OnInit( AppContext* app ) {
    app->isRunning = true;
    app->rendererAPI.Initialize();
    app->rendererAPI.SetClearColor( 1.0f, 0.5f, 0.5f, 1.0f );
    
    const char* openSansFilePath = "./resources/open_sans/OpenSans-Regular.ttf";
    Platform::File openSansFile = {};
    if(Platform::LoadFile(openSansFilePath, &openSansFile)) {
        app->defaultFontAtlas = {};
        if(Core::CreateFontAtlas(
            &openSansFile,
            64.0f,
            512, 512,
            ' ', '~',
            &app->defaultFontAtlas
        )) {
            LOG_INFO("App > \"%s\" successfully created!", app->defaultFontAtlas.fontName);
            if(!InitializeRenderContext( app )) {
                return false;
            }
        } else {
            LOG_ERROR("App > Failed to create font OpenSans!");
            return false;
        }
        Platform::FreeFile( &openSansFile );
    } else {
        LOG_ERROR("App > Failed to load OpenSans!");
        return false;
    }
    return true;
}

bool InitializeRenderContext( Core::AppContext* app ) {
    Core::RenderContext*   ctx = &app->renderContext;
    Platform::RendererAPI* api = &app->rendererAPI;
    Platform::File fontVertFile = {};
    if(!Platform::LoadFile( "./resources/shaders/font/font.glslVert", &fontVertFile )) {
        LOG_ERROR("App > Failed to load font vert file!");
        return false;
    }
    Platform::File fontFragFile = {};
    if(!Platform::LoadFile( "./resources/shaders/font/font.glslFrag", &fontFragFile )) {
        LOG_ERROR("App > Failed to load font frag file!");
        Platform::FreeFile( &fontVertFile );
        return false;
    }

    bool fontShaderResult = api->CreateShader(
        (char*)fontVertFile.data, fontVertFile.size,
        (char*)fontFragFile.data, fontFragFile.size,
        &ctx->fontShader
    );
    Platform::FreeFile( &fontVertFile );
    Platform::FreeFile( &fontFragFile );

    Platform::File boundsVertFile = {};
    if(!Platform::LoadFile( "./resources/shaders/bounds/bounds.glslVert", &boundsVertFile )) {
        LOG_ERROR("App > Failed to load bounds vert file!");
        return false;
    }
    Platform::File boundsFragFile = {};
    if(!Platform::LoadFile( "./resources/shaders/bounds/bounds.glslFrag", &boundsFragFile )) {
        LOG_ERROR("App > Failed to load bounds frag file!");
        Platform::FreeFile( &boundsVertFile );
        return false;
    }

    bool boundsShaderResult = api->CreateShader(
        (char*)boundsVertFile.data, boundsVertFile.size,
        (char*)boundsFragFile.data, boundsFragFile.size,
        &ctx->boundsShader
    );
    Platform::FreeFile( &boundsVertFile );
    Platform::FreeFile( &boundsFragFile );

    if(!boundsShaderResult) {
        LOG_ERROR("App > Failed to compile bounds shader!");
        return false;
    }
    if(!api->GetUniformID(
        &ctx->fontShader,
        "u_transform",
        &ctx->fontShaderUniformTransform
    )) {
        return false;
    }
    if(!api->GetUniformID(
        &ctx->fontShader,
        "u_fontCoords",
        &ctx->fontShaderUniformFontCoords
    )) {
        return false;
    }
    if(!api->GetUniformID(
        &ctx->fontShader,
        "u_color",
        &ctx->fontShaderUniformColor
    )) {
        return false;
    }
    if(!api->GetUniformID(
        &ctx->boundsShader,
        "u_transform",
        &ctx->boundsShaderUniformTransform
    )) {
        return false;
    }
    smath::vec4 defaultFontColor = smath::vec4::one();
    api->UniformVec4(
        &ctx->fontShader,
        ctx->fontShaderUniformColor,
        &defaultFontColor
    );

    smath::mat4 mat2d = smath::mat4::ortho(
        0.0f,
        (f32)app->windowDimensions.x,
        0.0f,
        (f32)app->windowDimensions.y
    );
    ctx->matrices2DBuffer = api->CreateUniformBuffer(
        sizeof( smath::mat4 ),
        mat2d.valuePtr()
    );
    api->UniformBufferSetBindingPoint(
        &ctx->matrices2DBuffer,
        RENDERER_2D_MATRICES_BINDING_POINT
    );

    api->SetUnPackAlignment( RENDERER_PACK_ALIGNMENT_1 );
    ctx->fontAtlasTexture = api->CreateTexture2D(
        app->defaultFontAtlas.width,
        app->defaultFontAtlas.height,
        app->defaultFontAtlas.bitmap,
        Platform::TextureFormat::R,
        Platform::DataType::UNSIGNED_BYTE,
        Platform::TextureWrapMode::CLAMP,
        Platform::TextureWrapMode::CLAMP,
        Platform::TextureMinFilter::LINEAR,
        Platform::TextureMagFilter::LINEAR
    );
    api->SetUnPackAlignment( RENDERER_PACK_ALIGNMENT_DEFAULT );

    Platform::Free( app->defaultFontAtlas.bitmap );
    app->defaultFontAtlas.bitmap = nullptr;

    f32 fontVertices[] = {
        /*POSITION*/ 0.0f, 1.0f, /*UV*/ 0.0f, 1.0f,
        /*POSITION*/ 1.0f, 1.0f, /*UV*/ 1.0f, 1.0f,
        /*POSITION*/ 0.0f, 0.0f, /*UV*/ 0.0f, 0.0f,
        /*POSITION*/ 1.0f, 0.0f, /*UV*/ 1.0f, 0.0f
    };
    const usize FONT_VERTEX_COUNT = 16;
    u16 fontIndices[] = {
        2, 1, 0,
        1, 2, 3
    };
    const usize FONT_INDEX_COUNT = 6;

    ctx->fontVertexArray = api->CreateVertexArray();
    api->UseVertexArray( &ctx->fontVertexArray );

    Platform::VertexBufferElement fontVertexElements[] = {
        { Platform::DataStructure::VEC4, Platform::DataType::FLOAT, false }
    };
    const usize FONT_VERTEX_ELEMENTS = 1;
    Platform::VertexBufferLayout fontVertexLayout = Platform::CreateVertexBufferLayout(
        FONT_VERTEX_ELEMENTS,
        fontVertexElements
    );

    Platform::VertexBuffer fontVertexBuffer = api->CreateVertexBuffer(
        FONT_VERTEX_COUNT * sizeof(f32),
        &fontVertices,
        fontVertexLayout
    );
    api->VertexArrayBindVertexBuffer(
        &ctx->fontVertexArray,
        fontVertexBuffer
    );

    Platform::IndexBuffer fontIndexBuffer = api->CreateIndexBuffer(
        FONT_INDEX_COUNT,
        fontIndices,
        Platform::DataType::UNSIGNED_SHORT
    );
    api->VertexArrayBindIndexBuffer(
        &ctx->fontVertexArray,
        fontIndexBuffer
    );

    // NOTE(alicia): bounds mesh

    f32 boundsVertices[] = {
        /*POSITION*/ 0.0f, 1.0f,
        /*POSITION*/ 1.0f, 1.0f,
        /*POSITION*/ 0.0f, 0.0f,
        /*POSITION*/ 1.0f, 0.0f
    };
    const usize BOUNDS_VERTEX_COUNT = 8;
    u16 boundsIndices[] = {
        2, 1, 0,
        1, 2, 3
    };
    const usize BOUNDS_INDEX_COUNT = 6;

    ctx->boundsVertexArray = api->CreateVertexArray();
    api->UseVertexArray( &ctx->boundsVertexArray );

    Platform::VertexBufferElement boundsVertexElements[] = {
        { Platform::DataStructure::VEC2, Platform::DataType::FLOAT, false }
    };
    const usize BOUNDS_VERTEX_ELEMENTS = 1;
    Platform::VertexBufferLayout boundsVertexLayout = Platform::CreateVertexBufferLayout(
        BOUNDS_VERTEX_ELEMENTS,
        boundsVertexElements
    );

    Platform::VertexBuffer boundsVertexBuffer = api->CreateVertexBuffer(
        BOUNDS_VERTEX_COUNT * sizeof(f32),
        &boundsVertices,
        boundsVertexLayout
    );
    api->VertexArrayBindVertexBuffer(
        &ctx->boundsVertexArray,
        boundsVertexBuffer
    );

    Platform::IndexBuffer boundsIndexBuffer = api->CreateIndexBuffer(
        BOUNDS_INDEX_COUNT,
        boundsIndices,
        Platform::DataType::UNSIGNED_SHORT
    );
    api->VertexArrayBindIndexBuffer(
        &ctx->boundsVertexArray,
        boundsIndexBuffer
    );

    return true;
}

Core::AppContext Core::CreateContext() {
    AppContext app = {};
    return app;
}
void Core::OnClose( AppContext* app ) {
    app->isRunning = false;
    Core::FreeFontAtlas( &app->defaultFontAtlas );
    app->rendererAPI.DeleteTextures2D(
        RENDER_CONTEXT_TEXTURE_COUNT,
        &app->renderContext.fontAtlasTexture
    );
    app->rendererAPI.DeleteShaders(
        RENDER_CONTEXT_SHADER_COUNT,
        &app->renderContext.fontShader
    );
    app->rendererAPI.DeleteUniformBuffers(
        RENDER_CONTEXT_UNIFORM_BUFFER_COUNT,
        &app->renderContext.matrices2DBuffer
    );
    app->rendererAPI.DeleteVertexArrays(
        RENDER_CONTEXT_VERTEX_ARRAY_COUNT,
        &app->renderContext.fontVertexArray
    );
}

void Render( Core::AppContext* app ) {
    app->rendererAPI.ClearBuffer();

    smath::vec2 textPos = smath::vec2::one() * 0.5f;
    smath::vec4 textCol = smath::vec4::one();
    RenderText(
        "Hello World",
        &textPos,
        1.0f,
        &textCol,
        TextAlignment::CENTER_CENTER,
        &app->defaultFontAtlas,
        app
    );

    app->rendererAPI.SwapBuffers();
}

void RenderCharacter(
    Platform::RendererAPI* api,
    Core::RenderContext* ctx,
    Core::FontMetrics* charMetrics,
    smath::vec2* charPosition,
    smath::vec2* pixelPosition,
    f32 scale
);

void RenderText(
    const char* text,
    smath::vec2* screenPos,
    f32 scale,
    smath::vec4* color,
    TextAlignment alignment,
    Core::FontAtlas* fontAtlas,
    Core::AppContext* app
) {
    usize textLen = stringLen( text );

    Platform::RendererAPI* api = &app->rendererAPI;
    Core::RenderContext* ctx   = &app->renderContext;

    api->SetBlendingEnable( true );
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
        screenPos->x * (f32)app->windowDimensions.x,
        screenPos->y * (f32)app->windowDimensions.y
    );

    f32 originX = pixelPosition.x;
    f32 yOffset = 0.0f;
    f32 textWidth = 0.0f;
    ucycles( textLen ) {
        char currentChar = text[i];
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
        case TextAlignment::CENTER_CENTER:
        case TextAlignment::CENTER_TOP:
        case TextAlignment::CENTER_BOTTOM: {
            originX -= textWidth / 2.0f;
        } break;
        case TextAlignment::RIGHT_CENTER:
        case TextAlignment::RIGHT_TOP:
        case TextAlignment::RIGHT_BOTTOM: {
            originX -= textWidth;
        } break;
        default: break;
    }

    switch( alignment ) {
        case TextAlignment::LEFT_CENTER:
        case TextAlignment::CENTER_CENTER:
        case TextAlignment::RIGHT_CENTER: {
            yOffset = -( ( fontAtlas->pointSize / 2.0f ) * scale );
        } break;
        case TextAlignment::LEFT_TOP:
        case TextAlignment::CENTER_TOP:
        case TextAlignment::RIGHT_TOP: {
            yOffset = -( fontAtlas->pointSize * scale );
        } break;
        default: break;
    }

    ucycles( textLen ) {
        char currentChar = text[i];
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

    api->SetBlendingEnable( false );
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

void Core::OnResolutionUpdate( AppContext* app, i32 width, i32 height ) {
    if(!app->isRunning) {
        return;
    }

    app->rendererAPI.SetViewport( width, height );
    app->windowDimensions.x = width;
    app->windowDimensions.y = height;

    smath::mat4 ortho = smath::mat4::ortho( 0.0f, (f32)width, 0.0f, (f32)height );
    app->rendererAPI.UniformBufferData(
        &app->renderContext.matrices2DBuffer,
        sizeof(smath::mat4),
        ortho.valuePtr()
    );

    Render( app );
}

void Core::OnAppActivated( AppContext* app ) {
    LOG_INFO("App active");
    UNUSED_PARAM(app);
}
void Core::OnAppDeactivated( AppContext* app ) {
    LOG_INFO("App inactive");
    UNUSED_PARAM(app);
}

void LoadMesh( Core::AppContext* app ) {
    Platform::File meshFile = {};
    if( Platform::UserLoadFile( "Load Mesh", &meshFile ) ) {
        Platform::FreeFile( &meshFile );
    }
    app->input = {};
}

void LoadAlbedo( Core::AppContext* app ) {
    Platform::File albedoFile = {};
    if( Platform::UserLoadFile( "Load Albedo Texture", &albedoFile ) ) {
        Platform::FreeFile( &albedoFile );
    }
    app->input = {};
}

void LoadNormal( Core::AppContext* app ) {
    Platform::File normalFile = {};
    if( Platform::UserLoadFile( "Load Normal Texture", &normalFile ) ) {
        Platform::FreeFile( &normalFile );
    }
    app->input = {};
}

void LoadSpecular( Core::AppContext* app ) {
    Platform::File specularFile = {};
    if( Platform::UserLoadFile( "Load Specular Texture", &specularFile ) ) {
        Platform::FreeFile( &specularFile );
    }
    app->input = {};
}
