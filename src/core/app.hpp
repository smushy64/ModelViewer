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

// forward declarations
enum class Anchor;
enum class ButtonState;
typedef void (*ButtonCallbackFN)(void* params);
class UserInterface;

inline const char* PROGRAM_TITLE     = "Model Viewer | Version 0.2 ";
inline const usize PROGRAM_TITLE_LEN = 27;

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

#define RENDERER_2D_MATRICES_BINDING_POINT 0

#define RENDER_CONTEXT_FONT_TEXTURE_UNIT 0
#define RENDER_CONTEXT_TEXTURE_COUNT 1
#define RENDER_CONTEXT_SHADER_COUNT 2
#define RENDER_CONTEXT_UNIFORM_BUFFER_COUNT 1
#define RENDER_CONTEXT_VERTEX_ARRAY_COUNT 2
struct RenderContext {
    Platform::Texture2D fontAtlasTexture;

    Platform::Shader fontShader;
    Platform::Shader boundsShader;

    Platform::UniformBuffer matrices2DBuffer;

    Platform::VertexArray fontVertexArray;
    Platform::VertexArray boundsVertexArray;

    i32 fontShaderUniformTransform;
    i32 fontShaderUniformFontCoords;
    i32 fontShaderUniformColor;

    i32 boundsShaderUniformTransform;

    smath::ivec2 viewport;
};

struct AppContext {
    UserInterface* ui;
    bool isRunning;
    Time time;
    Platform::RendererAPI rendererAPI;
    Input input;
    smath::ivec2 windowDimensions;
    Core::FontAtlas defaultFontAtlas;
    RenderContext renderContext;
};

AppContext CreateContext();
bool OnInit( AppContext* app );
void OnClose( AppContext* app );
void OnUpdate( AppContext* app );
void OnAppActivated( AppContext* app );
void OnAppDeactivated( AppContext* app );
void OnResolutionUpdate( AppContext* app, i32 width, i32 height );

} // namespace Core


