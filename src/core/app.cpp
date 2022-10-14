#include "app.hpp"
#include "platform/event.hpp"
#include "platform/renderer.hpp"
#include "global.hpp"
#include "debug.hpp"
#include <glm/vec4.hpp>

void InitializeApp( const Core::AppData& data ) {
    data.renderer->SetClearColor( glm::vec4( 0.2f ) );
}

void UpdateApp() {
    Platform::ProcessWindowEvents();
}

void RenderApp( Platform::Renderer* renderer ) {
    renderer->ClearBuffer();{

        // render scene

    } renderer->SwapBuffer();
}

void Core::RunApp( AppData data ) {
    InitializeApp( data );
    while( PROGRAM_RUNNING ) {
        UpdateApp();
        RenderApp( data.renderer );
    }
    // cleanup
    delete( data.renderer );
}
