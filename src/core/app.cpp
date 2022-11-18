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

void Render( Core::AppContext* app ) {
    app->rendererAPI.ClearBuffer();
    app->rendererAPI.SwapBuffers();
    return;

    smath::vec2 textPos = smath::vec2::one() * 0.5f;
    smath::vec4 textCol = smath::vec4::one();
    RenderText(
        "Hello World",
        &textPos,
        1.0f,
        &textCol,
        TextAlignment::LEFT_BOTTOM,
        &app->defaultFontAtlas,
        app
    );
}

void Core::OnUpdate( AppContext* app ) {
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
        Render(app);
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
            16.0f,
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

    bool fontShaderResult = app->rendererAPI.CreateShader(
        (char*)fontVertFile.data, fontVertFile.size,
        (char*)fontFragFile.data, fontFragFile.size,
        &app->renderContext.fontShader
    );
    Platform::FreeFile( &fontVertFile );
    Platform::FreeFile( &fontFragFile );

    if(!fontShaderResult) {
        LOG_ERROR("App > Failed to compile font shader!");
        return false;
    }

    smath::mat4 iden = smath::mat4::identity();
    app->renderContext.matrices2DBuffer = app->rendererAPI.CreateUniformBuffer(
        sizeof( smath::mat4 ),
        &iden
    );
    app->rendererAPI.UniformBufferSetBindingPoint(
        &app->renderContext.matrices2DBuffer,
        RENDERER_2D_MATRICES_BINDING_POINT
    );

    app->rendererAPI.SetUnPackAlignment( RENDERER_PACK_ALIGNMENT_1 );
    app->renderContext.fontAtlasTexture = app->rendererAPI.CreateTexture2D(
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
    app->rendererAPI.SetUnPackAlignment( RENDERER_PACK_ALIGNMENT_DEFAULT );

    Platform::Free( app->defaultFontAtlas.bitmap );

    app->rendererAPI.GetUniformID(
        &app->renderContext.fontShader,
        "u_color",
        &app->renderContext.fontShaderUniformColor
    );

    Platform::VertexArray va  = app->rendererAPI.CreateVertexArray();
    f32 vertices[] = {
        0.5f, 0.0f, -0.5f,
        0.5f, 0.0f, -0.5f,
    };
    Platform::VertexBufferElement elements[] = {
        { Platform::DataStructure::VEC3, Platform::DataType::FLOAT, false }
    };
    Platform::VertexBufferLayout layout = Platform::CreateVertexBufferLayout( 1, elements );
    Platform::VertexBuffer vb = app->rendererAPI.CreateVertexBuffer( 6ULL * sizeof(f32), &vertices, layout );
    app->rendererAPI.VertexArrayBindVertexBuffer( &va, vb );

    u32 indices[] = { 0, 1 };
    Platform::IndexBuffer ib = app->rendererAPI.CreateIndexBuffer( 2ULL * sizeof(u32), indices, Platform::DataType::UNSIGNED_INT );
    app->rendererAPI.VertexArrayBindIndexBuffer( &va, ib );

    app->rendererAPI.DeleteVertexArrays( 1, &va );


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
        &app->renderContext.fontAtlasTexture,
        RENDER_CONTEXT_TEXTURE_COUNT
    );
    app->rendererAPI.DeleteShaders(
        &app->renderContext.fontShader,
        RENDER_CONTEXT_SHADER_COUNT
    );
    app->rendererAPI.DeleteUniformBuffers(
        &app->renderContext.matrices2DBuffer,
        RENDER_CONTEXT_UNIFORM_BUFFER_COUNT
    );
}

void RenderText(
    const char* text,
    smath::vec2* screenPos,
    f32 scale,
    smath::vec4* color,
    TextAlignment alignment,
    Core::FontAtlas* fontAtlas,
    Core::AppContext* app
) {
    UNUSED_PARAM(text);
    UNUSED_PARAM(screenPos);
    UNUSED_PARAM(scale);
    UNUSED_PARAM(color);
    UNUSED_PARAM(alignment);
    UNUSED_PARAM(fontAtlas);
    UNUSED_PARAM(app);
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
