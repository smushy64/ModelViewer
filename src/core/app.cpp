/**
 * Description:  Application Definition
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 14, 2022 
*/
#include "app.hpp"
#include "util.hpp"
#include "ui.hpp"
#include "image.hpp"
#include "obj.hpp"

using Platform::KeyCode;

void LoadMesh( void* app );
void LoadAlbedo( void* app );
void LoadNormal( void* app );
void LoadSpecular( void* app );
bool InitializeRenderContext( Core::AppContext* app );

void Render( Core::AppContext* app ) {

    Platform::RendererAPI* api = &app->rendererAPI;
    Core::RenderContext*   ctx = &app->renderContext;

    ctx->camera.recalculateView();

    api->UniformBufferData(
        &ctx->matrices3DBuffer,
        sizeof( smath::mat4 ) * ctx->camera.matrixCount(),
        ctx->camera.matrices()
    );

    api->UniformBufferSubData(
        &ctx->dataBuffer,
        0,
        sizeof( smath::vec3 ),
        ctx->camera.position.ptr()
    );

    api->ClearBuffer();

    api->UseShader( &ctx->blinnPhongShader );
    api->UseTexture2D( &ctx->modelAlbedoTexture, RENDER_CONTEXT_ALBEDO_TEXTURE_UNIT );
    api->UseTexture2D( &ctx->modelSpecularTexture, RENDER_CONTEXT_SPECULAR_TEXTURE_UNIT );
    api->UseTexture2D( &ctx->modelNormalTexture, RENDER_CONTEXT_NORMAL_TEXTURE_UNIT );

    api->UseVertexArray( &ctx->modelVertexArray );
    api->DrawVertexArray( &ctx->modelVertexArray );

    api->SetBlendingEnable( true );
    app->ui->renderInterface( api, ctx );
    api->SetBlendingEnable( false );

    api->SwapBuffers();
}

void Core::OnUpdate( AppContext* app ) {
    if( Platform::IsAppActive() ) {
        Platform::ResetCursorStyle();
        
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

        Core::camera* camera = &app->renderContext.camera;
        smath::vec2* eulerCameraRotation = &app->renderContext.eulerCameraRotation;
        smath::vec3* targetCameraPosition = &app->renderContext.targetCameraPosition;
        smath::vec2* targetCameraRotationEuler = &app->renderContext.targetCameraRotationEuler;

        if( app->input.rightMouse ) {
            Platform::SetCursorLocked( true );
            smath::vec2 lateral = smath::vec2(0.0f);
            f32 vertical = 0.0f;
            if( app->input.IsKeyDown( KeyCode::W ) ) {
                lateral.y = 1.0f;
            } else if( app->input.IsKeyDown( KeyCode::S ) ) {
                lateral.y = -1.0f;
            }

            if( app->input.IsKeyDown( KeyCode::A ) ) {
                lateral.x = 1.0f;
            } else if( app->input.IsKeyDown( KeyCode::D ) ) {
                lateral.x = -1.0f;
            }

            if( app->input.IsKeyDown( KeyCode::E ) ) {
                vertical = 1.0f;
            } else if( app->input.IsKeyDown( KeyCode::Q ) ) {
                vertical = -1.0f;
            }

            smath::normalize( lateral );

            smath::vec3 movement =
                (lateral.x * camera->right) +
                (vertical * smath::vec3::up()) +
                (lateral.y * camera->forward);

            *targetCameraPosition += movement * app->time.deltaTime;

            smath::vec2 deltaMouse = app->input.screenMousePos - app->input.lastScreenMousePos;
            deltaMouse *= app->time.deltaTime * CAMERA_SENSITIVITY;
            
            *targetCameraRotationEuler -= deltaMouse;
            targetCameraRotationEuler->y = smath::clamp(
                targetCameraRotationEuler->y,
                -MAX_CAMERA_Y_ROTATION,
                MAX_CAMERA_Y_ROTATION
            );

        } else {
            Platform::SetCursorLocked( false );
            app->ui->updateInterface( &app->input );
            if( app->input.IsKeyDown( KeyCode::R ) ) {
                *targetCameraPosition = DEFAULT_CAMERA_POSITION;
                *targetCameraRotationEuler = DEFAULT_CAMERA_EROTATION;
                *eulerCameraRotation  = DEFAULT_CAMERA_EROTATION;
            }
        }
        *eulerCameraRotation = smath::lerp(
            *eulerCameraRotation,
            *targetCameraRotationEuler,
            app->time.deltaTime * CAMERA_ROT_LERP_SPEED
        );
        smath::quat rotX = smath::quat::angleAxis( eulerCameraRotation->y, camera->right );
        smath::quat rotY = smath::quat::angleAxis( eulerCameraRotation->x, smath::vec3::up() );
        camera->rotation = rotX * rotY;
        camera->position = smath::lerp(
            camera->position,
            *targetCameraPosition,
            app->time.deltaTime * CAMERA_MOVE_LERP_SPEED
        );

        f32 lastFOVDelta = app->renderContext.cameraFOVDelta;
        app->renderContext.cameraFOVDelta -= (f32)app->input.mouseWheel * app->time.deltaTime * CAMERA_ZOOM_SPEED;
        if( app->input.middleMouse ) {
            app->renderContext.cameraFOVDelta = 0.0f;
        }

        app->renderContext.cameraTargetFOV = CAMERA_FOV + app->renderContext.cameraFOVDelta;
        if( app->renderContext.cameraTargetFOV > CAMERA_MAX_FOV ) {
            app->renderContext.cameraTargetFOV = CAMERA_MAX_FOV;
            app->renderContext.cameraFOVDelta = lastFOVDelta;
        } else if( app->renderContext.cameraTargetFOV < CAMERA_MIN_FOV ) {
            app->renderContext.cameraTargetFOV = CAMERA_MIN_FOV;
            app->renderContext.cameraFOVDelta = lastFOVDelta;
        }

        f32 lastFOV = camera->fovRad;
        camera->fovRad = smath::lerp(
            camera->fovRad,
            app->renderContext.cameraTargetFOV,
            app->time.deltaTime * CAMERA_ZOOM_LERP_SPEED
        );
        if( lastFOV != camera->fovRad ) {
            camera->recalculateProjection();
        }

        camera->recalculateBasis();
        Render(app);
    }
}

const smath::vec3 CLEAR_COLORS[] = {
    { 1.0f, 0.5f, 0.5f },
    { 0.67f, 0.48f, 0.93f },
    { 0.96f, 0.77f, 0.36f },
    { 0.58f, 0.92f, 0.9f }
};
const usize NUMBER_OF_CLEAR_COLORS = 3;

bool Core::OnInit( AppContext* app ) {
    app->isRunning = true;
    app->rendererAPI.Initialize();
    app->renderContext.viewport = app->windowDimensions;

    u64 random = (u64)smath::rand( Platform::GetSystemTime() ) % (u64)NUMBER_OF_CLEAR_COLORS;
    const smath::vec3& clearColor = CLEAR_COLORS[random];

    app->rendererAPI.SetClearColor(
        clearColor.x,
        clearColor.y,
        clearColor.z,
        1.0f
    );
    
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
            app->ui = new Core::UserInterface( &app->defaultFontAtlas );
            app->ui->onResolutionChange( &app->windowDimensions );
            app->ui->loadMeshButton().setCallback( LoadMesh, app );
            app->ui->loadAlbedoTextureButton().setCallback( LoadAlbedo, app );
            app->ui->loadSpecularTextureButton().setCallback( LoadSpecular, app );
            app->ui->loadNormalTextureButton().setCallback( LoadNormal, app );
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

    ctx->camera.position      = Core::DEFAULT_CAMERA_POSITION;
    ctx->targetCameraPosition = ctx->camera.position;
    ctx->camera.rotation      = Core::DEFAULT_CAMERA_ROTATION;
    ctx->targetCameraRotationEuler = Core::DEFAULT_CAMERA_EROTATION;
    ctx->eulerCameraRotation  = Core::DEFAULT_CAMERA_EROTATION;

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
    if(!fontShaderResult) {
        LOG_ERROR("App > Failed to compile font shader!");
        return false;
    }

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

    Platform::File blinnPhongVertFile = {};
    if(!Platform::LoadFile( "./resources/shaders/blinn_phong/blinn_phong.glslVert", &blinnPhongVertFile )) {
        LOG_ERROR("App > Failed to load blinnPhong vert file!");
        return false;
    }
    Platform::File blinnPhongFragFile = {};
    if(!Platform::LoadFile( "./resources/shaders/blinn_phong/blinn_phong.glslFrag", &blinnPhongFragFile )) {
        LOG_ERROR("App > Failed to load blinnPhong frag file!");
        Platform::FreeFile( &blinnPhongVertFile );
        return false;
    }

    bool blinnPhongShaderResult = api->CreateShader(
        (char*)blinnPhongVertFile.data, blinnPhongVertFile.size,
        (char*)blinnPhongFragFile.data, blinnPhongFragFile.size,
        &ctx->blinnPhongShader
    );
    Platform::FreeFile( &blinnPhongVertFile );
    Platform::FreeFile( &blinnPhongFragFile );
    if(!blinnPhongShaderResult) {
        LOG_ERROR("App > Failed to compile blinnPhong shader!");
        return false;
    }

    if(!api->GetUniformID(
        &ctx->blinnPhongShader,
        "u_transform",
        &ctx->blinnPhongUniformTransform
    )) {
        return false;
    }
    smath::mat4 bpTransform = smath::mat4::translate( 0.0f, 0.0f, 2.0f );
    api->UniformMat4(
        &ctx->blinnPhongShader,
        ctx->blinnPhongUniformTransform,
        &bpTransform
    );
    if(!api->GetUniformID(
        &ctx->blinnPhongShader,
        "u_normalMat",
        &ctx->blinnPhongUniformNormalMat
    )) {
        return false;
    }
    if(!api->GetUniformID(
        &ctx->blinnPhongShader,
        "u_normalTexturePresent",
        &ctx->blinnPhongUniformNormalTexturePresent
    )) {
        return false;
    }

    api->UniformInt(
        &ctx->blinnPhongShader,
        ctx->blinnPhongUniformNormalTexturePresent,
        0
    );

    smath::mat3 bpNormalMat = smath::mat3::identity();
    if(!smath::mat3::normalMat( bpTransform, bpNormalMat )) {
        LOG_DEBUG("normal mat failed!");
    }
    api->UniformMat3(
        &ctx->blinnPhongShader,
        ctx->blinnPhongUniformNormalMat,
        &bpNormalMat
    );
    if(!api->GetUniformID(
        &ctx->blinnPhongShader,
        "u_surfaceTint",
        &ctx->blinnPhongUniformSurfaceTint
    )) {
        return false;
    }
    smath::vec3 bpTint = smath::vec3(1.0f);
    api->UniformVec3(
        &ctx->blinnPhongShader,
        ctx->blinnPhongUniformSurfaceTint,
        &bpTint
    );
    if(!api->GetUniformID(
        &ctx->blinnPhongShader,
        "u_glossiness",
        &ctx->blinnPhongUniformGlossiness
    )) {
        return false;
    }
    api->UniformFloat(
        &ctx->blinnPhongShader,
        ctx->blinnPhongUniformGlossiness,
        32.0f
    );

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
        mat2d.ptr()
    );
    api->UniformBufferSetBindingPoint(
        &ctx->matrices2DBuffer,
        RENDERER_2D_MATRICES_BINDING_POINT
    );

    ctx->matrices3DBuffer = api->CreateUniformBuffer(
        sizeof( smath::mat4 ) * ctx->camera.matrixCount(),
        ctx->camera.matrices()
    );
    api->UniformBufferSetBindingPoint(
        &ctx->matrices3DBuffer,
        RENDERER_3D_MATRICES_BINDING_POINT
    );

    f32 dataBuffer[] = {
        ctx->camera.position.x,
        ctx->camera.position.y,
        ctx->camera.position.z,
        0.0f,
        ctx->camera.clippingPlanes.x,
        ctx->camera.clippingPlanes.y,
    };
    ctx->dataBuffer = api->CreateUniformBuffer(
        sizeof(f32) * 6,
        &dataBuffer[0]
    );
    api->UniformBufferSetBindingPoint(
        &ctx->dataBuffer,
        RENDERER_DATA_BINDING_POINT
    );

    ctx->lights.ambient.color         = smath::vec3( 0.01f );
    ctx->lights.directional.direction = smath::normalize(smath::vec3::up() + smath::vec3::right() + smath::vec3::forward());
    ctx->lights.directional.diffuse   = smath::vec3(1.0f);

    ctx->lightsBuffer = api->CreateUniformBuffer(
        320,
        &ctx->lights
    );
    api->UniformBufferSetBindingPoint(
        &ctx->lightsBuffer,
        RENDERER_LIGHTS_BINDING_POINT
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

    u8 tex[] = {
        255, 255, 255
    };
    ctx->modelAlbedoTexture = api->CreateTexture2D(
        1, 1, tex,
        Platform::TextureFormat::RGB,
        Platform::DataType::UNSIGNED_BYTE,
        Platform::TextureWrapMode::CLAMP,
        Platform::TextureWrapMode::CLAMP,
        Platform::TextureMinFilter::NEAREST_MIPMAP_NEAREST,
        Platform::TextureMagFilter::NEAREST
    );
    ctx->modelSpecularTexture = api->CreateTexture2D(
        1, 1, tex,
        Platform::TextureFormat::RGB,
        Platform::DataType::UNSIGNED_BYTE,
        Platform::TextureWrapMode::CLAMP,
        Platform::TextureWrapMode::CLAMP,
        Platform::TextureMinFilter::NEAREST_MIPMAP_NEAREST,
        Platform::TextureMagFilter::NEAREST
    );
    ctx->modelNormalTexture = api->CreateTexture2D(
        1, 1, tex,
        Platform::TextureFormat::RGB,
        Platform::DataType::UNSIGNED_BYTE,
        Platform::TextureWrapMode::CLAMP,
        Platform::TextureWrapMode::CLAMP,
        Platform::TextureMinFilter::NEAREST_MIPMAP_NEAREST,
        Platform::TextureMagFilter::NEAREST
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

    Core::vertex bpVertices[] = {
        { /*POS*/ {  0.5f,  0.5f, -0.5f }, /*UV*/ { 1.0f, 1.0f }, /*NORM*/ { 0.0, 0.0f, -1.0f }, {}, {} },
        { /*POS*/ {  0.5f, -0.5f, -0.5f }, /*UV*/ { 1.0f, 0.0f }, /*NORM*/ { 0.0, 0.0f, -1.0f }, {}, {} },
        { /*POS*/ { -0.5f,  0.5f, -0.5f }, /*UV*/ { 0.0f, 1.0f }, /*NORM*/ { 0.0, 0.0f, -1.0f }, {}, {} },
        { /*POS*/ { -0.5f, -0.5f, -0.5f }, /*UV*/ { 0.0f, 0.0f }, /*NORM*/ { 0.0, 0.0f, -1.0f }, {}, {} },

        { /*POS*/ {  0.5f,  0.5f, 0.5f }, /*UV*/ { 1.0f, 1.0f }, /*NORM*/ { 0.0f, 0.0f, 1.0f }, {}, {} },
        { /*POS*/ {  0.5f, -0.5f, 0.5f }, /*UV*/ { 1.0f, 0.0f }, /*NORM*/ { 0.0f, 0.0f, 1.0f }, {}, {} },
        { /*POS*/ { -0.5f,  0.5f, 0.5f }, /*UV*/ { 0.0f, 1.0f }, /*NORM*/ { 0.0f, 0.0f, 1.0f }, {}, {} },
        { /*POS*/ { -0.5f, -0.5f, 0.5f }, /*UV*/ { 0.0f, 0.0f }, /*NORM*/ { 0.0f, 0.0f, 1.0f }, {}, {} },

        { /*POS*/ { -0.5f,  0.5f,  0.5f, }, /*UV*/ { 1.0f, 1.0f }, /*NORM*/ { -1.0f, 0.0f, 0.0f, }, {}, {} },
        { /*POS*/ { -0.5f,  0.5f, -0.5f, }, /*UV*/ { 1.0f, 0.0f }, /*NORM*/ { -1.0f, 0.0f, 0.0f, }, {}, {} },
        { /*POS*/ { -0.5f, -0.5f,  0.5f, }, /*UV*/ { 0.0f, 1.0f }, /*NORM*/ { -1.0f, 0.0f, 0.0f, }, {}, {} },
        { /*POS*/ { -0.5f, -0.5f, -0.5f, }, /*UV*/ { 0.0f, 0.0f }, /*NORM*/ { -1.0f, 0.0f, 0.0f, }, {}, {} },

        { /*POS*/ {  0.5f,  0.5f,  0.5f, }, /*UV*/ { 1.0f, 1.0f }, /*NORM*/ { 1.0f, 0.0f, 0.0f, }, {}, {} },
        { /*POS*/ {  0.5f,  0.5f, -0.5f, }, /*UV*/ { 1.0f, 0.0f }, /*NORM*/ { 1.0f, 0.0f, 0.0f, }, {}, {} },
        { /*POS*/ {  0.5f, -0.5f,  0.5f, }, /*UV*/ { 0.0f, 1.0f }, /*NORM*/ { 1.0f, 0.0f, 0.0f, }, {}, {} },
        { /*POS*/ {  0.5f, -0.5f, -0.5f, }, /*UV*/ { 0.0f, 0.0f }, /*NORM*/ { 1.0f, 0.0f, 0.0f, }, {}, {} },

        { /*POS*/ {  0.5f,  0.5f,  0.5f, }, /*UV*/ { 1.0f, 1.0f }, /*NORM*/ { 0.0f, 1.0f, 0.0f, }, {}, {} },
        { /*POS*/ {  0.5f,  0.5f, -0.5f, }, /*UV*/ { 1.0f, 0.0f }, /*NORM*/ { 0.0f, 1.0f, 0.0f, }, {}, {} },
        { /*POS*/ { -0.5f,  0.5f,  0.5f, }, /*UV*/ { 0.0f, 1.0f }, /*NORM*/ { 0.0f, 1.0f, 0.0f, }, {}, {} },
        { /*POS*/ { -0.5f,  0.5f, -0.5f, }, /*UV*/ { 0.0f, 0.0f }, /*NORM*/ { 0.0f, 1.0f, 0.0f, }, {}, {} },

        { /*POS*/ {  0.5f, -0.5f,  0.5f, }, /*UV*/ { 1.0f, 1.0f }, /*NORM*/ { 0.0f, -1.0f, 0.0f, }, {}, {} },
        { /*POS*/ {  0.5f, -0.5f, -0.5f, }, /*UV*/ { 1.0f, 0.0f }, /*NORM*/ { 0.0f, -1.0f, 0.0f, }, {}, {} },
        { /*POS*/ { -0.5f, -0.5f,  0.5f, }, /*UV*/ { 0.0f, 1.0f }, /*NORM*/ { 0.0f, -1.0f, 0.0f, }, {}, {} },
        { /*POS*/ { -0.5f, -0.5f, -0.5f, }, /*UV*/ { 0.0f, 0.0f }, /*NORM*/ { 0.0f, -1.0f, 0.0f, }, {}, {} },
    };

    Core::calculateTangentBasis( sizeof(bpVertices) / sizeof(Core::vertex), bpVertices );

    u32 bpIndices[] {
        0, 1, 2,
        3, 2, 1,

        6, 5, 4,
        5, 6, 7,

         8,  9, 10,
        11, 10,  9,

        14, 13, 12,
        13, 14, 15,

        16, 17, 18,
        19, 18, 17,

        22, 21, 20,
        21, 22, 23,
    };

    ctx->modelVertexArray = api->CreateVertexArray();
    api->UseVertexArray( &ctx->modelVertexArray );

    Platform::VertexBuffer bpvb = api->CreateVertexBuffer(
        sizeof(bpVertices),
        &bpVertices,
        Core::vertexLayout()
    );
    api->VertexArrayBindVertexBuffer( &ctx->modelVertexArray, bpvb );

    Platform::IndexBuffer bpib = api->CreateIndexBuffer(
        sizeof(bpIndices) / sizeof(u32),
        bpIndices,
        Platform::DataType::UNSIGNED_INT
    );
    api->VertexArrayBindIndexBuffer( &ctx->modelVertexArray, bpib );

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
    delete app->ui;
}

void Core::OnResolutionUpdate( AppContext* app, i32 width, i32 height ) {
    if(!app->isRunning) {
        return;
    }

    app->rendererAPI.SetViewport( width, height );
    app->windowDimensions.x = width;
    app->windowDimensions.y = height;
    app->renderContext.viewport = app->windowDimensions;

    smath::mat4 ortho = smath::mat4::ortho( 0.0f, (f32)width, 0.0f, (f32)height );
    app->rendererAPI.UniformBufferData(
        &app->renderContext.matrices2DBuffer,
        sizeof(smath::mat4),
        ortho.ptr()
    );

    app->renderContext.camera.aspectRatio = (f32)width / (f32)height;
    app->renderContext.camera.recalculateProjection();

    Render( app );
}

void Core::OnAppActivated( AppContext* app ) {
    LOG_INFO("App active");
    UNUSED_PARAM(app);
}
void Core::OnAppDeactivated( AppContext* app ) {
    LOG_INFO("App inactive");
    app->input = {};
    Platform::SetCursorLocked(false);
}

void LoadMesh( void* params ) {
    Core::AppContext* app = (Core::AppContext*)params;
    Platform::File meshFile = {};
    if( Platform::UserLoadFile( "Load Mesh", &meshFile ) ) {

        Platform::VertexArray va = {};
        if( Core::ParseOBJ( &meshFile, &va, &app->rendererAPI ) ) {
            app->rendererAPI.DeleteVertexArrays( 1, &app->renderContext.modelVertexArray );
            app->renderContext.modelVertexArray = va;
        }

        Platform::FreeFile( &meshFile );
    }
    app->input = {};
}

void LoadAlbedo( void* params ) {
    Core::AppContext* app = (Core::AppContext*)params;
    Platform::File albedoFile = {};
    if( Platform::UserLoadFile( "Load Albedo Texture", &albedoFile ) ) {

        Core::Image image = {};
        if( Core::ReadImage( albedoFile.size, albedoFile.data, &image ) ) {

            app->rendererAPI.DeleteTextures2D( 1, &app->renderContext.modelAlbedoTexture );

            Platform::TextureFormat textureFormat = Platform::TextureFormat::R;
            switch( image.colorComponentCount ) {
                case 1: textureFormat = Platform::TextureFormat::R; break;
                case 2: textureFormat = Platform::TextureFormat::RG; break;
                case 3: textureFormat = Platform::TextureFormat::RGB; break;
                case 4: textureFormat = Platform::TextureFormat::RGBA; break;
                default: break;
            }

            app->renderContext.modelAlbedoTexture = app->rendererAPI.CreateTexture2D(
                image.width,
                image.height,
                image.data,
                textureFormat,
                Platform::DataType::UNSIGNED_BYTE,
                Platform::TextureWrapMode::CLAMP,
                Platform::TextureWrapMode::CLAMP,
                Platform::TextureMinFilter::LINEAR,
                Platform::TextureMagFilter::LINEAR
            );

            Core::FreeImage( &image );

        }

        Platform::FreeFile( &albedoFile );
    }
    app->input = {};
}

void LoadNormal( void* params ) {
    Core::AppContext* app = (Core::AppContext*)params;
    Platform::File normalFile = {};
    if( Platform::UserLoadFile( "Load Normal Texture", &normalFile ) ) {

        Core::Image image = {};
        if( Core::ReadImage( normalFile.size, normalFile.data, &image ) ) {

            app->rendererAPI.DeleteTextures2D( 1, &app->renderContext.modelNormalTexture );

            Platform::TextureFormat textureFormat = Platform::TextureFormat::R;
            switch( image.colorComponentCount ) {
                case 1: textureFormat = Platform::TextureFormat::R; break;
                case 2: textureFormat = Platform::TextureFormat::RG; break;
                case 3: textureFormat = Platform::TextureFormat::RGB; break;
                case 4: textureFormat = Platform::TextureFormat::RGBA; break;
                default: break;
            }

            app->renderContext.modelNormalTexture = app->rendererAPI.CreateTexture2D(
                image.width,
                image.height,
                image.data,
                textureFormat,
                Platform::DataType::UNSIGNED_BYTE,
                Platform::TextureWrapMode::CLAMP,
                Platform::TextureWrapMode::CLAMP,
                Platform::TextureMinFilter::NEAREST,
                Platform::TextureMagFilter::NEAREST
            );
            app->rendererAPI.UniformInt(
                &app->renderContext.blinnPhongShader,
                app->renderContext.blinnPhongUniformNormalTexturePresent,
                1
            );

            Core::FreeImage( &image );

        }

        Platform::FreeFile( &normalFile );
    }
    app->input = {};
}

void LoadSpecular( void* params ) {
    Core::AppContext* app = (Core::AppContext*)params;
    Platform::File specularFile = {};
    if( Platform::UserLoadFile( "Load Specular Texture", &specularFile ) ) {

        Core::Image image = {};
        if( Core::ReadImage( specularFile.size, specularFile.data, &image ) ) {

            app->rendererAPI.DeleteTextures2D( 1, &app->renderContext.modelSpecularTexture );

            Platform::TextureFormat textureFormat = Platform::TextureFormat::R;
            switch( image.colorComponentCount ) {
                case 1: textureFormat = Platform::TextureFormat::R; break;
                case 2: textureFormat = Platform::TextureFormat::RG; break;
                case 3: textureFormat = Platform::TextureFormat::RGB; break;
                case 4: textureFormat = Platform::TextureFormat::RGBA; break;
                default: break;
            }

            app->renderContext.modelSpecularTexture = app->rendererAPI.CreateTexture2D(
                image.width,
                image.height,
                image.data,
                textureFormat,
                Platform::DataType::UNSIGNED_BYTE,
                Platform::TextureWrapMode::CLAMP,
                Platform::TextureWrapMode::CLAMP,
                Platform::TextureMinFilter::NEAREST,
                Platform::TextureMagFilter::NEAREST
            );

            Core::FreeImage( &image );

        }

        Platform::FreeFile( &specularFile );
    }
    app->input = {};
}

