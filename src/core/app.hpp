/**
 * Description:  Application Definition
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 14, 2022 
*/
#pragma once
#include "pch.hpp"
#include "platform/renderer.hpp"
#include "platform/io.hpp"
#include "font.hpp"

namespace Core {

struct Time {
    f32 elapsedTime;
    f32 deltaTime;
};

struct Input {
    /// @brief 0-width/height mouse position
    /// x-axis: 0 is left and width is right 
    /// y-axis: 0 is bottom and height is top
    smath::ivec2 pixelMousePos;
    /// @brief 0.0-1.0 mouse position. 
    /// x-axis: 0.0f is left and 1.0f is right 
    /// y-axis: 0.0f is bottom and 1.0f is top
    smath::vec2  screenMousePos;
    bool keys[Platform::KEY_COUNT];

    /// @brief Check if key is down 
    bool IsKeyDown( Platform::KeyCode keycode ) const { return keys[ (usize)keycode ]; }
    /// @brief Set key state
    void SetKey( Platform::KeyCode keycode, bool state ) { keys[(usize)keycode] = state; }

    bool leftMouse;
    bool rightMouse;
    bool middleMouse;
    i32 mouseWheel;
};

struct RenderContext {
    Platform::Texture2D fontAtlas;
    Platform::Shader   fontShader;
};

struct AppContext {
    bool isRunning;
    Time time;
    Platform::RendererAPI rendererAPI;
    Input input;
    smath::ivec2 windowDimensions;
    Core::FontAtlas defaultFontAtlas;
    RenderContext renderContext;
};

AppContext CreateContext();
void OnInit( AppContext* app );
void OnClose( AppContext* app );
void OnUpdate( AppContext* app );
void OnAppActivated( AppContext* app );
void OnAppDeactivated( AppContext* app );
void OnResolutionUpdate( AppContext* app, i32 width, i32 height );

} // namespace Core


