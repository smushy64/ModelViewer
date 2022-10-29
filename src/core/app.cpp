#include "global.hpp"
#include "debug.hpp"

#include "platform/io.hpp"
#include "platform/event.hpp"
#include "platform/renderer.hpp"
#include "platform/time.hpp"
#include "platform/pointer.hpp"

#include "app.hpp"
#include "text.hpp"
#include "ui.hpp"
#include "collision.hpp"
#include "macro.hpp"

#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>

Core::FontAtlas DEFAULT_FONT;
bool LoadDefaultFont();

struct AppUI {
    Core::UI::Canvas canvas;
    usize versionLabelID, loadButtonID, quitButtonID;
};
AppUI APPUI;
void InitializeApp( Core::AppData& data );

f32 yaw = 0.0f;
void UpdateApp( Core::AppData& data ) {
    Platform::ResetPointerStyle();
    APPUI.canvas.GetLabelButton(APPUI.loadButtonID).UpdateState( data.input );
    APPUI.canvas.GetLabelButton(APPUI.quitButtonID).UpdateState( data.input );

    bool rightMouse = data.input.mouseButtons[(usize)Core::MouseCode::RIGHT];
    if( rightMouse ) {
        Platform::LockPointer();
    } else {
        Platform::UnlockPointer();
    }

    if( rightMouse ) {
        auto vertical = glm::angleAxis(
            data.input.screenSpaceMouseDelta.x,
            data.camera->Up()
        );
        auto horizontal = glm::angleAxis(
            data.input.screenSpaceMouseDelta.y,
            data.camera->Right()
        );
        yaw += data.input.screenSpaceMouseDelta.x;
        data.camera->Rotate( vertical * horizontal );
    }

    f32 panX = 0.0f, panY = 0.0f;
    if( data.input.keys[(usize)Core::KeyCode::A] ) {
        panX = -1.0f;
    } else if( data.input.keys[(usize)Core::KeyCode::D] ) {
        panX = 1.0f;
    }

    if( data.input.keys[(usize)Core::KeyCode::W] ) {
        panY = 1.0f;
    } else if( data.input.keys[(usize)Core::KeyCode::S] ) {
        panY = -1.0f;
    }

    glm::vec3 vertical   = data.camera->Up()    * ( panY * data.deltaTime );
    glm::vec3 horizontal = glm::angleAxis( yaw, glm::vec3( 0.0f, 1.0f, 0.0f ) ) *
        ( glm::vec3( -panX, 0.0f, 0.0f ) * data.deltaTime );
    data.camera->Move( vertical + horizontal );

}

void RenderScene( Core::AppData& data ) {
    data.sh->UseShader();
    data.va->UseArray();
    data.renderer->API()->SetActiveTexture(0);
    data.albedo->UseTexture();
    data.renderer->API()->SetActiveTexture(1);
    data.specular->UseTexture();
    data.renderer->API()->DrawVertexArray( data.va );
}

void RenderUI( Platform::Renderer* renderer ) {
    renderer->RenderText( APPUI.canvas.GetLabel(APPUI.versionLabelID) );
    renderer->RenderTextButton( APPUI.canvas.GetLabelButton(APPUI.loadButtonID) );
    renderer->RenderTextButton( APPUI.canvas.GetLabelButton(APPUI.quitButtonID) );
}

void RenderApp( Core::AppData& data ) {
    data.renderer->API()->ClearBuffer();{

        RenderScene( data );
        RenderUI( data.renderer );

    } data.renderer->API()->SwapBuffers();
}

void HandleEvents( Core::AppData& data );
void Core::RunApp( AppData& data ) {
    InitializeApp( data );
    
    f32 lastElapsedTime = 0.0f;
    while( data.isRunning ) {
        f32 elapsedTime = Platform::ElapsedTime();
        data.deltaTime = elapsedTime - lastElapsedTime;
        lastElapsedTime = elapsedTime;

        Platform::ProcessWindowEvents( data );
        HandleEvents( data );

        UpdateApp( data );
        data.camera->RecalculateView();
        data.camera->RecalculateProjection();
        data.camera->RecalculateBasisVectors();
        data.renderer->BufferCameraPosition( data.camera->GetCameraPoint() );
        data.renderer->BufferClippingFields( data.camera->GetClippingFields() );

        RenderApp( data );
    }

    // cleanup
    delete( data.albedo );
    delete( data.specular );
    delete( data.lights );
    delete( data.camera );
    delete( data.renderer );
    delete( data.va );
    delete( data.sh );
    delete( DEFAULT_FONT.texture );
}

void HandleEvents( Core::AppData& data ) {
    using namespace Core;

    bool mouseEventHappened = false;

    while( !data.events.IsEmpty() ) {
        Event* event = data.events.Pop();

    switch( event->GetType() ) {
        case Event::Type::QUIT: {
            data.isRunning = false;
            LOG_INFO("App > Quit Event Handled");
        } break;
        case Event::Type::WINDOW_RESIZE: {
            WindowResizeEvent* resizeEvent = (WindowResizeEvent*)event;
            data.screenResolution = resizeEvent->GetResolution();
            data.renderer->ResolutionChanged( data.screenResolution );
            data.camera->SetAspectRatio( data.screenResolution );

            APPUI.canvas.OnResolutionChange( data.screenResolution );
        } break;
        case Event::Type::KEY_DOWN: {
            KeyDown* keydown = (KeyDown*)event;
            data.input.keys[ (usize)keydown->GetKeycode() ] = true;
        } break;
        case Event::Type::KEY_UP: {
            KeyUp* keyup = (KeyUp*)event;
            data.input.keys[ (usize)keyup->GetKeycode() ] = false;
        } break;
        case Event::Type::MOUSE_POSITION: {
            mouseEventHappened = true;
            MousePositionEvent* mousePos = (MousePositionEvent*)event;
            data.input.lastPixelMouse       = data.input.pixelMouse;
            data.input.lastScreenSpaceMouse = data.input.screenSpaceMouse;
            data.input.pixelMouse       = mousePos->GetPixelPosition();
            data.input.screenSpaceMouse = mousePos->GetScreenPosition();

            data.input.pixelMouseDelta = data.input.pixelMouse - data.input.lastPixelMouse;
            data.input.screenSpaceMouseDelta = data.input.screenSpaceMouse - data.input.lastScreenSpaceMouse;
        } break;
        case Event::Type::MOUSE_DOWN: {
            MouseDown* mouseDown = (MouseDown*)event;
            data.input.mouseButtons[(usize)mouseDown->GetMouseCode()] = true;
        } break;
        case Event::Type::MOUSE_UP: {
            MouseUp* mouseDown = (MouseUp*)event;
            data.input.mouseButtons[(usize)mouseDown->GetMouseCode()] = false;
        } break;
        case Event::Type::MOUSE_SCROLL: {
            MouseScroll* mouseScroll = (MouseScroll*)event;
            data.input.mouseScroll = mouseScroll->GetMouseScrollDirection();
            data.camera->Zoom( CAMERA_ZOOM_FACTOR * data.deltaTime * -(f32)mouseScroll->GetMouseScrollDirection() );
        } break;
        case Event::Type::FILE_LOADED: {
            Core::FileLoadedEvent* fileLoaded = (Core::FileLoadedEvent*)event;
            Platform::File file = fileLoaded->GetFile();
            Platform::FreeFile(file);
        } break;
        case Event::Type::MOUSE_LEAVE: {
            std::fill( std::begin( data.input.mouseButtons ), std::end( data.input.mouseButtons ), false );
        } break;

        default: break;
    }

        delete( event );
    }

    if(!mouseEventHappened) {
        data.input.pixelMouseDelta       = glm::ivec2(0);
        data.input.screenSpaceMouseDelta = glm::vec2(0.0f);
    }
}

bool LoadDefaultFont() {
    Platform::File openSansFile = Platform::LoadFile( "./resources/open_sans/OpenSans-Regular.ttf" );
    Core::FontAtlasSettings openSansSettings = {};
    openSansSettings.fromChar  = ' ';
    openSansSettings.toChar    = '~';
    openSansSettings.pointSize = 64.0f;
    DEFAULT_FONT = {};
    bool fontResult = Core::CreateFontAtlas( openSansFile, openSansSettings, DEFAULT_FONT );
    Platform::FreeFile( openSansFile );

    return fontResult;
}

void QuitCallback( void* param ) {

#ifdef DEBUG
    if( param == nullptr ) {
        LOG_ERROR( "Quit Button Callback > Parameter is null!" );
        return;
    }
#endif

    Core::AppData* appData = (Core::AppData*)param;
    appData->isRunning = false;
}

void LoadCallback( void* param ) {
#ifdef DEBUG
    if( param == nullptr ) {
        LOG_ERROR( "Load Callback > Parameter is null!" );
        return;
    }
#endif

    Core::AppData* appData = (Core::AppData*)param;
    // set mouse to false so that it stops registering clicks during the pop up
    appData->input.mouseButtons[(usize)Core::MouseCode::LEFT] = false;

    auto file = Platform::LoadFilePopup();
    if( file.size == 0 ) {
        LOG_WARN("Load Callback > No file loaded!");
        return;
    }
    
    appData->events.Push( new Core::FileLoadedEvent( file ) );
}

void InitializeApp( Core::AppData& data ) {

    data.renderer->Initialize();
    data.camera = new Core::Camera();
    data.lights = new Core::Lights();
    data.lights->GetAmbientLight().SetColor( glm::vec3(0.1f, 0.05f, 0.08f) );
    data.lights->UploadAmbient();

    data.lights->GetPointLight(0).Activate();
    data.lights->GetPointLight(0).SetPosition( glm::vec3( 2.0f, -3.0f, -1.5f ) );
    data.lights->GetPointLight(0).SetDiffuse( glm::vec3( 0.0f, 0.0f, 0.5f ) );
    data.lights->UploadPointLights();

    if( !Platform::InitializeTimer() ) {
        LOG_ERROR("App > Failed to initialize timer!");
        data.isRunning = false;
        return;
    }

    if( !LoadDefaultFont() ) {
        LOG_ERROR( "App > Failed to load default font!" );
        data.isRunning = false;
        return;
    }

    APPUI.canvas = Core::UI::Canvas();

    auto versionLabel = Core::UI::Label( PROGRAM_TITLE, DEFAULT_FONT );
    versionLabel.SetAnchorY( Core::YAnchor::BOTTOM );
    versionLabel.SetPosition( glm::vec2(0.01f, 0.0125f) );
    versionLabel.SetScale( 0.35f );

    APPUI.versionLabelID = APPUI.canvas.PushLabel( versionLabel );

    f32 menuXOffset = 0.99f;
    f32 menuScale = 0.65f;

    auto loadButton = Core::UI::LabelButton( "Load Mesh", DEFAULT_FONT, &data.screenResolution );
    loadButton.SetAnchorX( Core::XAnchor::RIGHT );
    loadButton.SetPosition( glm::vec2( menuXOffset, 0.1f ) );
    loadButton.SetScale( menuScale );
    loadButton.SetCallback( LoadCallback );
    loadButton.SetCallbackParameter( (void*)&data );

    APPUI.loadButtonID = APPUI.canvas.PushLabelButton( loadButton );

    auto quitButton = Core::UI::LabelButton( "Quit Program", DEFAULT_FONT, &data.screenResolution );
    quitButton.SetAnchorX( Core::XAnchor::RIGHT );
    quitButton.SetPosition( glm::vec2( menuXOffset, 0.05f ) );
    quitButton.SetScale( menuScale );
    quitButton.SetCallback( QuitCallback );
    quitButton.SetCallbackParameter( (void*)&data );

    APPUI.quitButtonID = APPUI.canvas.PushLabelButton( quitButton );

    data.renderer->UploadFontAtlasBitmap( DEFAULT_FONT );
    data.renderer->API()->SetClearColor( glm::vec4( 0.15f ) );

    /* Cube */ {
        f32 verts[] = {
            // front
            /* Positions */ -0.5,  0.5,  0.5,  /* UVs */  0.0,  1.0, /* Normals */  0.0, 0.0,  1.0,
            /* Positions */  0.5,  0.5,  0.5,  /* UVs */  1.0,  1.0, /* Normals */  0.0, 0.0,  1.0,
            /* Positions */ -0.5, -0.5,  0.5,  /* UVs */  0.0,  0.0, /* Normals */  0.0, 0.0,  1.0,
            /* Positions */  0.5, -0.5,  0.5,  /* UVs */  1.0,  0.0, /* Normals */  0.0, 0.0,  1.0,

            // back
            /* Positions */ -0.5,  0.5, -0.5, /* UVs */  0.0,  1.0, /* Normals */  0.0, 0.0, -1.0,
            /* Positions */  0.5,  0.5, -0.5, /* UVs */  1.0,  1.0, /* Normals */  0.0, 0.0, -1.0,
            /* Positions */ -0.5, -0.5, -0.5, /* UVs */  0.0,  0.0, /* Normals */  0.0, 0.0, -1.0,
            /* Positions */  0.5, -0.5, -0.5, /* UVs */  1.0,  0.0, /* Normals */  0.0, 0.0, -1.0,

            // left
            /* Positions */ -0.5,  0.5, -0.5, /* UVs */  0.0,  1.0, /* Normals */ -1.0, 0.0,  0.0,
            /* Positions */ -0.5,  0.5,  0.5, /* UVs */  1.0,  1.0, /* Normals */ -1.0, 0.0,  0.0,
            /* Positions */ -0.5, -0.5, -0.5, /* UVs */  0.0,  0.0, /* Normals */ -1.0, 0.0,  0.0,
            /* Positions */ -0.5, -0.5,  0.5, /* UVs */  1.0,  0.0, /* Normals */ -1.0, 0.0,  0.0,

            // right
            /* Positions */  0.5,  0.5, -0.5, /* UVs */  0.0,  1.0, /* Normals */  1.0, 0.0,  0.0,
            /* Positions */  0.5,  0.5,  0.5, /* UVs */  1.0,  1.0, /* Normals */  1.0, 0.0,  0.0,
            /* Positions */  0.5, -0.5, -0.5, /* UVs */  0.0,  0.0, /* Normals */  1.0, 0.0,  0.0,
            /* Positions */  0.5, -0.5,  0.5, /* UVs */  1.0,  0.0, /* Normals */  1.0, 0.0,  0.0,

            // top
            /* Positions */ -0.5,  0.5,  0.5, /* UVs */  0.0,  1.0, /* Normals */  0.0, 1.0,  0.0,
            /* Positions */  0.5,  0.5,  0.5, /* UVs */  1.0,  1.0, /* Normals */  0.0, 1.0,  0.0,
            /* Positions */ -0.5,  0.5, -0.5, /* UVs */  0.0,  0.0, /* Normals */  0.0, 1.0,  0.0,
            /* Positions */  0.5,  0.5, -0.5, /* UVs */  1.0,  0.0, /* Normals */  0.0, 1.0,  0.0,

            // bottom
            /* Positions */ -0.5, -0.5,  0.5, /* UVs */  0.0,  1.0, /* Normals */   0.0, -1.0, 0.0,
            /* Positions */  0.5, -0.5,  0.5, /* UVs */  1.0,  1.0, /* Normals */   0.0, -1.0, 0.0,
            /* Positions */ -0.5, -0.5, -0.5, /* UVs */  0.0,  0.0, /* Normals */   0.0, -1.0, 0.0,
            /* Positions */  0.5, -0.5, -0.5, /* UVs */  1.0,  0.0, /* Normals */   0.0, -1.0, 0.0,
        };
        const usize VERT_COUNT = 192;

        u16 idx[] = {
            0, 1, 2,
            1, 3, 2,

            4, 5, 6,
            5, 7, 6,

            8,  9, 10,
            9, 11, 10,

            12, 13, 14,
            13, 15, 14,

            16, 17, 18,
            17, 19, 18,

            20, 21, 22,
            21, 23, 22,
        };
        const usize IDX_COUNT = 36;

        data.va = Platform::VertexArray::New();
        data.va->UseArray();

        auto vBuffer = Platform::VertexBuffer::New( VERT_COUNT * sizeof(f32), &verts );
        vBuffer->SetLayout(Platform::BufferLayout({
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
            )
        }));

        data.va->AddVertexBuffer( vBuffer );

        auto iBuffer = Platform::IndexBuffer::New(
            Platform::BufferDataType::USHORT,
            IDX_COUNT,
            &idx
        );

        data.va->SetIndexBuffer( iBuffer );
    }

    /* Blinn Phong Shader */ {
        const char* vpath = "./resources/shaders/blinn_phong/blinn_phong.glslVert";
        const char* fpath = "./resources/shaders/blinn_phong/blinn_phong.glslFrag";

        Platform::TextFile vsrc = Platform::LoadTextFile( vpath );
        Platform::TextFile fsrc = Platform::LoadTextFile( fpath );

        if( vsrc.size == 0 || fsrc.size == 0 ) {
            LOG_ERROR("App > Failed to load model shaders!");
            data.isRunning = false;
            return;
        }

        data.sh = Platform::Shader::New( vsrc.contents, fsrc.contents );
        UniformID transformID;
        data.sh->UseShader();
        data.sh->GetUniform( "u_transform", transformID );
        glm::mat4 transform = glm::mat4(1.0f);
        data.sh->UniformMat4( transformID, transform );

        glm::mat3 normalMat = glm::transpose( glm::inverse( transform ) );
        UniformID normalMatID;
        data.sh->GetUniform( "u_normalMat", normalMatID );
        data.sh->UniformMat3( normalMatID, normalMat );

        UniformID glossinessID;
        data.sh->GetUniform( "u_glossiness", glossinessID );
        data.sh->UniformFloat( glossinessID, 32.0f );

        UniformID surfaceTintID;
        data.sh->GetUniform( "u_surfaceTint", surfaceTintID );
        data.sh->UniformVec3( surfaceTintID, glm::vec3(1.0f) );

        UniformID albedoID, specularID;
        data.sh->GetUniform( "u_albedoSampler", albedoID );
        data.sh->GetUniform( "u_specularSampler", specularID );
        data.sh->UniformInt( albedoID, 0 );
        data.sh->UniformInt( specularID, 1 );

        u32 albedo = 
            (u8)255 << 0  |
            (u8)255 << 8  |
            (u8)255 << 16 |
            (u8)255 << 24
        ;
        u32 specular = 
            (u8)255 << 0  |
            (u8)255 << 8  |
            (u8)255 << 16 |
            (u8)255 << 24
        ;
        data.albedo = Platform::Texture2D::New(
            glm::ivec2( 1 ),
            &albedo,
            Platform::TextureFormat::RGBA,
            Platform::TextureInternalFormat::RGBA,
            Platform::BufferDataType::UBYTE
        );
        data.specular = Platform::Texture2D::New(
            glm::ivec2( 1 ),
            &specular,
            Platform::TextureFormat::RGBA,
            Platform::TextureInternalFormat::RGBA,
            Platform::BufferDataType::UBYTE
        );

    }

    data.camera->SetRotation( glm::vec3( TO_RAD( 30.0f ), TO_RAD( 30.0f ), 0.0f ) );

}
