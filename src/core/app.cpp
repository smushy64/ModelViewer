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

    api->UniformBufferData(
        &ctx->matrices3DBuffer,
        sizeof( smath::mat4 ) * ctx->camera.matrixCount(),
        ctx->camera.matrices()
    );

    api->UniformBufferSubData(
        &ctx->dataBuffer,
        0,
        sizeof( smath::vec3 ),
        &ctx->camera.position[0]
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
    Core::Input*         input  = &app->input;
    Core::RenderContext* ctx    = &app->renderContext;
    Core::camera*        camera = &ctx->camera;
    Core::Time*          time   = &app->time;

    if( Platform::IsAppActive() ) {

        if( input->rightMouse ) {
            Platform::SetCursorLocked( true );

            f32 zoomDelta = (f32)input->mouseWheel * time->deltaTime;
            ctx->targetCameraFOV = smath::clamp(ctx->targetCameraFOV - zoomDelta, CAMERA_MIN_FOV, CAMERA_MAX_FOV);

            smath::vec2 lateralMovement  = {};
            f32         verticalMovement = 0.0f;
            if( input->IsKeyDown( KeyCode::A ) ) {
                lateralMovement.x = 1.0f;
            } else if( input->IsKeyDown( KeyCode::D ) ) {
                lateralMovement.x = -1.0f;
            }

            if( input->IsKeyDown( KeyCode::W ) ) {
                lateralMovement.y = 1.0f;
            } else if( input->IsKeyDown( KeyCode::S ) ) {
                lateralMovement.y = -1.0f;
            }

            if( input->IsKeyDown( KeyCode::E ) ) {
                verticalMovement = 1.0f;
            } else if( input->IsKeyDown( KeyCode::Q ) ) {
                verticalMovement = -1.0f;
            }

            smath::vec3 movement = normalize((lateralMovement.x * camera->right) + (lateralMovement.y * camera->forward)) +
                (verticalMovement  * smath::vec3::up());

            ctx->targetCameraPosition += movement * time->deltaTime;

            smath::vec2 deltaMouse = input->screenMousePos - input->lastScreenMousePos;
            deltaMouse *= time->deltaTime * CAMERA_SENSITIVITY;

            ctx->eulerCameraRotation.x -= deltaMouse.x;
            ctx->eulerCameraRotation.y -= deltaMouse.y;

            ctx->eulerCameraRotation.y = smath::clamp(
                ctx->eulerCameraRotation.y,
                -MAX_CAMERA_Y_ROTATION,
                MAX_CAMERA_Y_ROTATION
            );

            smath::quat rotX = smath::quat::angleAxis( ctx->eulerCameraRotation.x, smath::vec3::up() );
            smath::quat rotY = smath::quat::angleAxis( ctx->eulerCameraRotation.y, camera->right );

            ctx->targetCameraRotation = rotY * rotX;

        } else {
            Platform::ResetCursorStyle();
            Platform::SetCursorLocked( false );

            if( input->IsKeyDown( KeyCode::CTRL ) ) {
                if( input->IsKeyDown( KeyCode::M ) ) {
                    LoadMesh( app );
                } else if( input->IsKeyDown( KeyCode::A ) ) {
                    LoadAlbedo( app );
                } else if( input->IsKeyDown( KeyCode::G ) ) {
                    LoadSpecular( app );
                } else if( input->IsKeyDown( KeyCode::N ) ) {
                    LoadNormal( app );
                }
                
                if( input->IsKeyDown( KeyCode::R ) ) {
                    ctx->targetCameraPosition = DEFAULT_CAMERA_POSITION;
                    ctx->targetCameraRotation = DEFAULT_CAMERA_ROTATION;
                    ctx->eulerCameraRotation  = DEFAULT_CAMERA_EROTATION;
                    ctx->targetCameraFOV = CAMERA_FOV;
                }
            }

            app->ui->updateInterface( input );

        }

        camera->position = smath::lerp(
            camera->position,
            ctx->targetCameraPosition,
            time->deltaTime * CAMERA_MOVE_LERP_SPEED
        );

        camera->fovRad = smath::lerp(
            camera->fovRad,
            ctx->targetCameraFOV,
            time->deltaTime * CAMERA_ZOOM_LERP_SPEED
        );

        f32 cameraRotTargetRotDifference = smath::unsignedAngle( camera->rotation, ctx->targetCameraRotation );
        if( cameraRotTargetRotDifference > 0.00001f ) {
            f32 rotationLerpSpeed = smath::remap(
                CAMERA_MIN_FOV, CAMERA_MAX_FOV,
                CAMERA_MAX_ROT_LERP_SPEED, CAMERA_MIN_ROT_LERP_SPEED,
                camera->fovRad
            );
            camera->rotation = smath::lerp(
                camera->rotation,
                ctx->targetCameraRotation,
                time->deltaTime * rotationLerpSpeed
            );
        }

        camera->recalculateView();
        camera->recalculateProjection();
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

    u64 random = (u64)smath::rand( Platform::GetSystemTime() ) % (u64)NUMBER_OF_CLEAR_COLORS;
    const smath::vec3& clearColor = CLEAR_COLORS[random];

    api->SetClearColor(
        clearColor.x,
        clearColor.y,
        clearColor.z,
        1.0f
    );
    ctx->camera.position      = Core::DEFAULT_CAMERA_POSITION;
    ctx->targetCameraPosition = ctx->camera.position;
    ctx->camera.rotation      = Core::DEFAULT_CAMERA_ROTATION;
    ctx->targetCameraRotation = ctx->camera.rotation;
    ctx->eulerCameraRotation  = Core::DEFAULT_CAMERA_EROTATION;
    ctx->camera.aspectRatio   = (f32)(ctx->viewport.x / ctx->viewport.y);
    ctx->camera.fovRad        = Core::CAMERA_FOV;
    ctx->targetCameraFOV      = ctx->camera.fovRad;

    ctx->camera.recalculateView();
    ctx->camera.recalculateBasis();
    ctx->camera.recalculateProjection();

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

    ctx->lights.ambient.color         = smath::vec4(0.01f, 0.0096f, 0.0085f, 0.0f);
    ctx->lights.directional.direction = smath::vec4( smath::normalize(
        smath::vec3::up() + smath::vec3::right() + (smath::vec3::forward() * 1.2f)
    ));
    ctx->lights.directional.diffuse   = smath::vec4(0.9f, 0.95f, 1.0f, 0.0f) * 0.75f;

    ctx->lights.point0.isActive = true;
    ctx->lights.point0.position = smath::vec4( 1.0f, 1.6f, 0.5f, 0.0f );
    ctx->lights.point0.diffuse  = smath::vec4(1.0f, 0.94f, 0.89f, 0.0f);
    ctx->lights.point0.specular = ctx->lights.point0.diffuse * 0.5f;

    ctx->lights.point1.isActive = true;
    ctx->lights.point1.position = smath::vec4( -1.0f, -1.6f, 0.5f, 0.0f );
    ctx->lights.point1.diffuse  = smath::vec4(1.0f, 0.92f, 0.87f, 0.0f);
    ctx->lights.point1.specular = ctx->lights.point1.diffuse * 0.5f;

    ctx->lightsBuffer = api->CreateUniformBuffer(
        sizeof( Core::lightBuffer ),
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
                Platform::TextureMagFilter::LINEAR
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
                Platform::TextureMagFilter::LINEAR
            );

            Core::FreeImage( &image );

        }

        Platform::FreeFile( &specularFile );
    }
    app->input = {};
}

