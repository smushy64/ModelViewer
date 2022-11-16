/**
 * Description:  Application Definition
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 14, 2022 
*/
#include "app.hpp"

void Render( Core::AppContext* app ) {
    app->rendererAPI.ClearBuffer();
    app->rendererAPI.SwapBuffers();
}

void Core::OnUpdate( AppContext* app ) {
    Render(app);
}

void Core::OnInit( AppContext* app ) {
    app->isRunning = true;
    app->rendererAPI.Initialize();
    app->rendererAPI.SetClearColor( 1.0f, 0.5f, 0.5f, 1.0f );
}

Core::AppContext Core::CreateContext() {
    AppContext app = {};
    return app;
}
void Core::OnClose( AppContext* app ) {
    app->isRunning = false;
}

void Core::OnResolutionUpdate( AppContext* app, i32 width, i32 height ) {
    app->rendererAPI.SetViewport( width, height );
    Render( app );
}
