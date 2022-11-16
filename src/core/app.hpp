/**
 * Description:  Application Definition
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 14, 2022 
*/
#pragma once
#include "pch.hpp"
#include "time.hpp"
#include "platform/renderer.hpp"

namespace Core {

struct AppContext {
    bool isRunning;
    Time time;
    Platform::RendererAPI rendererAPI;
};

AppContext CreateContext();
void OnInit( AppContext* app );
void OnClose( AppContext* app );
void OnUpdate( AppContext* app );
void OnResolutionUpdate( AppContext* app, i32 width, i32 height );

} // namespace Core

