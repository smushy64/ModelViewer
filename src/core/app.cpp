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
#include "macro.hpp"
#include "image.hpp"
#include "obj.hpp"
#include "utils.hpp"

#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>

#include <string>
#include <sstream>

Core::FontAtlas DEFAULT_FONT;
bool LoadDefaultFont();

void InitializeApp( Core::AppData& data );
void Cleanup( Core::AppData& data );

f32 yaw = 0.0f;
void UpdateApp( Core::AppData& data ) {
    Platform::ResetPointerStyle();
    data.canvas.UpdateState( data.input );

    bool rightMouse = data.input.mouseButtons[(usize)Core::MouseCode::RIGHT];
    if( rightMouse ) {
        Platform::LockPointer();
    } else {
        Platform::UnlockPointer();
    }

    auto const& cameraUp = data.renderer->GetCamera()->Up();
    auto const& cameraRight = data.renderer->GetCamera()->Right();

    if( rightMouse ) {
        auto vertical = glm::angleAxis(
            data.input.screenSpaceMouseDelta.x,
            cameraUp
        );
        auto horizontal = glm::angleAxis(
            data.input.screenSpaceMouseDelta.y,
            cameraRight
        );
        yaw += data.input.screenSpaceMouseDelta.x;
        data.renderer->GetCamera()->Rotate( vertical * horizontal );
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

    glm::vec3 vertical   = cameraUp * ( panY * data.deltaTime );
    glm::vec3 horizontal = glm::angleAxis( yaw, glm::vec3( 0.0f, 1.0f, 0.0f ) ) *
        ( glm::vec3( panX, 0.0f, 0.0f ) * data.deltaTime );
    data.renderer->GetCamera()->Move( vertical + horizontal );

}

void RenderApp( Core::AppData& data ) {
    data.renderer->StartScene(); {

        data.renderer->DrawMesh( Platform::Renderer::Material::BLINNPHONG );
        data.canvas.Render( data.renderer );

    } data.renderer->EndScene();
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
        RenderApp( data );
    }

    // cleanup
    Cleanup(data);
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
            data.canvas.OnResolutionChange( data.screenResolution );
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
            data.renderer->GetCamera()->Zoom(
                CAMERA_ZOOM_FACTOR * data.deltaTime * -(f32)mouseScroll->GetMouseScrollDirection()
            );
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
    Core::AppData* appData = (Core::AppData*)param;
    appData->isRunning = false;
}

void LoadMeshCallback( void* param ) {
    Core::AppData* appData = (Core::AppData*)param;

    auto loadResult = Platform::LoadFilePopup();
    if( loadResult.contents != nullptr ) {
        auto text = Platform::FileToTextFile( loadResult );
        auto mesh = Core::ParseOBJ( text );

        if( mesh != nullptr ) {
            appData->renderer->SetMesh( mesh );
        }
    }

}

void LoadAlbedoCallback( void* param ) {
    Core::AppData* appData = (Core::AppData*)param;

    auto loadResult = Platform::LoadTexturePopup();
    if( loadResult != nullptr ) {
        appData->renderer->GetBlinnPhong()->SetAlbedo( loadResult );
    }
}

void LoadSpecularCallback( void* param ) {
    Core::AppData* appData = (Core::AppData*)param;

    auto loadResult = Platform::LoadTexturePopup();
    if( loadResult != nullptr ) {
        appData->renderer->GetBlinnPhong()->SetSpecular( loadResult );
    }
}

void LoadNormalCallback( void* param ) {
    Core::AppData* appData = (Core::AppData*)param;

    auto loadResult = Platform::LoadTexturePopup();
    if( loadResult != nullptr ) {
        appData->renderer->GetBlinnPhong()->SetNormal( loadResult );
    }
}

void InitializeApp( Core::AppData& data ) {

    data.renderer->Initialize();
    data.renderer->API()->ClearBuffer();
    data.renderer->API()->SwapBuffers();

    data.renderer->GetLights()->SetAmbientColor( glm::vec3(0.008f, 0.0f, 0.0f) );
    data.renderer->GetLights()->UploadAmbient();

    data.renderer->GetLights()->GetPointLight(0).Activate();
    data.renderer->GetLights()->GetPointLight(0).SetPosition( glm::vec3( 2.0f, 3.0f, -1.5f ) );
    data.renderer->GetLights()->GetPointLight(0).SetDiffuse( glm::vec3( 1.0f, 1.0f, 1.0f ) );
    data.renderer->GetLights()->UploadPointLights();

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

    data.canvas = Core::UI::Canvas();

    auto versionLabel = Core::UI::Label( Utils::GetProgramTitle(), DEFAULT_FONT );
    versionLabel.SetAnchorY( Core::YAnchor::BOTTOM );
    versionLabel.SetPosition( glm::vec2(0.01f, 0.0125f) );
    versionLabel.SetScale( 0.35f );
    data.canvas.PushLabel( versionLabel );

    f32 menuXOffset = 0.99f;
    f32 menuScale = 0.65f;

    auto loadAlbedoButton = Core::UI::LabelButton( "Load Albedo Texture", DEFAULT_FONT, &data.screenResolution );
    loadAlbedoButton.SetAnchorX( Core::XAnchor::RIGHT );
    loadAlbedoButton.SetPosition( glm::vec2( menuXOffset, 0.25f ) );
    loadAlbedoButton.SetScale( menuScale );
    loadAlbedoButton.SetCallback( LoadAlbedoCallback );
    loadAlbedoButton.SetCallbackParameter( (void*)&data );
    data.canvas.PushLabelButton( loadAlbedoButton );

    auto loadSpecularButton = Core::UI::LabelButton( "Load Specular Texture", DEFAULT_FONT, &data.screenResolution );
    loadSpecularButton.SetAnchorX( Core::XAnchor::RIGHT );
    loadSpecularButton.SetPosition( glm::vec2( menuXOffset, 0.2f ) );
    loadSpecularButton.SetScale( menuScale );
    loadSpecularButton.SetCallback( LoadSpecularCallback );
    loadSpecularButton.SetCallbackParameter( (void*)&data );
    data.canvas.PushLabelButton( loadSpecularButton );

    auto loadNormalButton = Core::UI::LabelButton( "Load Normal Texture", DEFAULT_FONT, &data.screenResolution );
    loadNormalButton.SetAnchorX( Core::XAnchor::RIGHT );
    loadNormalButton.SetPosition( glm::vec2( menuXOffset, 0.15f ) );
    loadNormalButton.SetScale( menuScale );
    loadNormalButton.SetCallback( LoadNormalCallback );
    loadNormalButton.SetCallbackParameter( (void*)&data );
    data.canvas.PushLabelButton( loadNormalButton );

    auto loadMeshButton = Core::UI::LabelButton( "Load Mesh", DEFAULT_FONT, &data.screenResolution );
    loadMeshButton.SetAnchorX( Core::XAnchor::RIGHT );
    loadMeshButton.SetPosition( glm::vec2( menuXOffset, 0.1f ) );
    loadMeshButton.SetScale( menuScale );
    loadMeshButton.SetCallback( LoadMeshCallback );
    loadMeshButton.SetCallbackParameter( (void*)&data );
    data.canvas.PushLabelButton( loadMeshButton );

    auto quitButton = Core::UI::LabelButton( "Quit Program", DEFAULT_FONT, &data.screenResolution );
    quitButton.SetAnchorX( Core::XAnchor::RIGHT );
    quitButton.SetPosition( glm::vec2( menuXOffset, 0.05f ) );
    quitButton.SetScale( menuScale );
    quitButton.SetCallback( QuitCallback );
    quitButton.SetCallbackParameter( (void*)&data );
    data.canvas.PushLabelButton( quitButton );

    data.renderer->UploadFontAtlasBitmap( DEFAULT_FONT );
    data.renderer->API()->SetClearColor( glm::vec4( 0.15f ) );

    data.renderer->GetCamera()->SetRotation(
        glm::vec3( TO_RAD( 30.0f ), TO_RAD( 210.0f ), 0.0f )
    );

}

void Cleanup( Core::AppData& data ) {
    delete( data.renderer );
    delete( DEFAULT_FONT.texture );
}

Core::SettingsINI Core::DefaultSettings() {
    const i32 DEFAULT_WINDOW_WIDTH  = 1280;
    const i32 DEFAULT_WINDOW_HEIGHT = 720;
    Core::SettingsINI result = {};
    result.width   = DEFAULT_WINDOW_WIDTH;
    result.height  = DEFAULT_WINDOW_HEIGHT;
    result.backend = Platform::BackendAPI::OPENGL;
    return result;
}

// Maybe rework this later? Works well enough as it is though

enum class Mode {
    NONE,
    GRAPHICS,
    RESOLUTION
};
bool ParseMode( const std::string& token, Mode& result ) {
    if( token == "[GRAPHICS]" ) {
        result = Mode::GRAPHICS;
        return true;
    } else if( token == "[RESOLUTION]" ) {
        result = Mode::RESOLUTION;
        return true;
    }

    return false;
}

enum class ResolutionMode {
    NONE,
    WIDTH,
    HEIGHT
};
bool ParseResolutionMode( const std::string& token, ResolutionMode& result ) {
    if( token == "width" ) {
        result = ResolutionMode::WIDTH;
    } else if( token == "height" ) {
        result = ResolutionMode::HEIGHT;
    } else {
        return false;
    }
    return true;
}

bool ParseBackend( const std::string& string, Platform::BackendAPI& result ) {
    if(
        string == "OPENGL" ||
        string == "OpenGL" ||
        string == "OpenGL Core 4.6"
    ) {
        result = Platform::BackendAPI::OPENGL;
    }
    else {
        return false;
    }
    return true;
}

void ParseToken(
    const std::string& token,
    Core::SettingsINI& result,
    Mode& mode,
    ResolutionMode& resolutionMode
) {
    if( ParseMode( token, mode ) ) {
        return;
    }

    switch( mode ) {
        case Mode::GRAPHICS: {
            Platform::BackendAPI api;
            if( ParseBackend( token, api ) ) {
                LOG_INFO("Settings Parser > API: %s", Platform::BackendToString(api));
                result.backend = api;
            }
        } break;
        case Mode::RESOLUTION: {
            ParseResolutionMode( token, resolutionMode );
            if( resolutionMode != ResolutionMode::NONE ) {
                i32 number;
                bool success = true;
                try {
                    number = std::stoi( token );
                } catch( std::invalid_argument const& ex ) { success = false; }
                catch( std::out_of_range const& ex ) { success = false; }

                if( success ) {
                    if( resolutionMode == ResolutionMode::WIDTH ) {
                        LOG_INFO("Settings Parser > Width: %i", number);
                        result.width = number;
                        resolutionMode = ResolutionMode::NONE;
                    } else if( resolutionMode == ResolutionMode::HEIGHT ) {
                        LOG_INFO("Settings Parser > Height: %i", number);
                        result.height = number;
                        resolutionMode = ResolutionMode::NONE;
                    }
                }
            }
        } break;
        default: return;
    }
}

bool Core::ParseSettings( const std::string& text, Core::SettingsINI& result ) {

    std::stringstream ss = std::stringstream(text);
    std::string line;
    Mode currentMode = Mode::NONE;
    ResolutionMode resolutionMode = ResolutionMode::NONE;
    while( std::getline( ss, line, '\n' ) ) {
        // skip comments
        if( line[0] == ';' ) {
            continue;
        }

        std::stringstream innerSS = std::stringstream(line);
        std::string token;
        while( std::getline( innerSS, token, ' ' ) ) {
            ParseToken( token, result, currentMode, resolutionMode );
        }

    }

    return true;
}
