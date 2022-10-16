#include "global.hpp"
#include "debug.hpp"

#include "platform/event.hpp"
#include "platform/renderer.hpp"
#include "platform/io.hpp"

#include "app.hpp"
#include "text.hpp"

#include <glm/vec4.hpp>

Core::FontAtlas DEFAULT_FONT;
bool LoadDefaultFont();

void InitializeApp( const Core::AppData& data ) {
    if(!data.renderer->Initialize()) {
        LOG_ERROR("App > Failed to initialize renderer!");
        PROGRAM_RUNNING = false;
        return;
    }

    if( LoadDefaultFont() ) {
        LOG_DEBUG( "App > Successfully Loaded Font \"%s\"!", DEFAULT_FONT.name.c_str() );
    } else {
        LOG_ERROR( "App > Failed to load default font!" );
        PROGRAM_RUNNING = false;
    }

    data.renderer->LoadFontAtlasBitmap( DEFAULT_FONT );
    data.renderer->UseFontAtlas( DEFAULT_FONT );
    data.renderer->SetClearColor( glm::vec4( 0.2f ) );
}

void UpdateApp() {
    Platform::ProcessWindowEvents();
}

void RenderApp( Platform::Renderer* renderer ) {
    renderer->ClearBuffer();{

        // render scene
        renderer->RenderText( "Hello World" );

    } renderer->SwapBuffer();
}

void Core::RunApp( AppData data ) {
    InitializeApp( data );
    while( PROGRAM_RUNNING ) {
        UpdateApp();
        RenderApp( data.renderer );
    }
    // cleanup
    if( DEFAULT_FONT.bitmap ) {
        free( DEFAULT_FONT.bitmap );
    }
    delete( data.renderer );
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
