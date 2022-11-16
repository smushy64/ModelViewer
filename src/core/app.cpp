/**
 * Description:  Application Definition
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 14, 2022 
*/
#include "app.hpp"
using Platform::KeyCode;

void LoadMesh( Core::AppContext* app );
void LoadAlbedo( Core::AppContext* app );
void LoadNormal( Core::AppContext* app );
void LoadSpecular( Core::AppContext* app );

void Render( Core::AppContext* app ) {
    app->rendererAPI.ClearBuffer();
    app->rendererAPI.SwapBuffers();
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
    app->windowDimensions.x = width;
    app->windowDimensions.y = height;
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
