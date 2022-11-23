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
#include "renderex.hpp"

namespace Core {

// forward declarations
enum class Anchor;
enum class ButtonState;
typedef void (*ButtonCallbackFN)(void* params);
class UserInterface;

inline const char* PROGRAM_TITLE     = "Model Viewer | Version 0.2 ";
inline const usize PROGRAM_TITLE_LEN = 27;

inline const f32 MAX_CAMERA_Y_ROTATION  = 0.959930888888889f;
inline const f32 CAMERA_SENSITIVITY     = 150.0f;
inline const f32 CAMERA_LERP_SPEED      = 50.0f;
inline const f32 CAMERA_FOV             = 1.1345f;
inline const f32 CAMERA_MIN_FOV         = 0.001f;
inline const f32 CAMERA_MAX_FOV         = 2.094f;
inline const f32 CAMERA_ZOOM_SPEED      = 10.0f;
inline const f32 CAMERA_ZOOM_LERP_SPEED = 25.0f;
inline const smath::vec3 DEFAULT_CAMERA_POSITION  = smath::vec3( 1.110f, 0.687f, 3.855f );
inline const smath::quat DEFAULT_CAMERA_ROTATION  = smath::quat( 0.270f, 0.057f, -0.941f, 0.197f );
inline const smath::vec2 DEFAULT_CAMERA_EROTATION = smath::vec2( -2.582f, 0.414f );

struct Time {
    f32 elapsedTime;
    f32 deltaTime;
};

struct Input {
    bool leftMouse;
    bool rightMouse;
    bool middleMouse;
    i32 mouseWheel;
    /// @brief 0-width/height mouse position
    /// x-axis: 0 is left and width is right 
    /// y-axis: 0 is bottom and height is top
    smath::ivec2 pixelMousePos;
    /// @brief 0.0-1.0 mouse position. 
    /// x-axis: 0.0f is left and 1.0f is right 
    /// y-axis: 0.0f is bottom and 1.0f is top
    smath::vec2  screenMousePos;
    /// @brief 0.0-1.0 mouse position. 
    /// x-axis: 0.0f is left and 1.0f is right 
    /// y-axis: 0.0f is bottom and 1.0f is top
    smath::vec2  lastScreenMousePos;
    bool keys[Platform::KEY_COUNT];

    /// @brief Check if key is down 
    bool IsKeyDown( Platform::KeyCode keycode ) const { return keys[ (usize)keycode ]; }
    /// @brief Set key state
    void SetKey( Platform::KeyCode keycode, bool state ) { keys[(usize)keycode] = state; }
};

#define RENDERER_2D_MATRICES_BINDING_POINT 0
#define RENDERER_3D_MATRICES_BINDING_POINT 1
#define RENDERER_LIGHTS_BINDING_POINT 2
#define RENDERER_DATA_BINDING_POINT 3

#define RENDER_CONTEXT_FONT_TEXTURE_UNIT 0

#define RENDER_CONTEXT_ALBEDO_TEXTURE_UNIT   0
#define RENDER_CONTEXT_SPECULAR_TEXTURE_UNIT 1
#define RENDER_CONTEXT_NORMAL_TEXTURE_UNIT   2

#define RENDER_CONTEXT_TEXTURE_COUNT 4
#define RENDER_CONTEXT_SHADER_COUNT 3
#define RENDER_CONTEXT_UNIFORM_BUFFER_COUNT 4
#define RENDER_CONTEXT_VERTEX_ARRAY_COUNT 3
struct RenderContext {
    Platform::Texture2D fontAtlasTexture;
    Platform::Texture2D modelAlbedoTexture;
    Platform::Texture2D modelSpecularTexture;
    Platform::Texture2D modelNormalTexture;

    Platform::Shader fontShader;
    Platform::Shader boundsShader;
    Platform::Shader blinnPhongShader;

    Platform::UniformBuffer matrices2DBuffer;
    Platform::UniformBuffer matrices3DBuffer;
    Platform::UniformBuffer lightsBuffer;
    Platform::UniformBuffer dataBuffer;

    Platform::VertexArray fontVertexArray;
    Platform::VertexArray boundsVertexArray;
    Platform::VertexArray modelVertexArray;

    i32 fontShaderUniformTransform;
    i32 fontShaderUniformFontCoords;
    i32 fontShaderUniformColor;

    i32 boundsShaderUniformTransform;

    i32 blinnPhongUniformTransform;
    i32 blinnPhongUniformNormalMat;
    i32 blinnPhongUniformSurfaceTint;
    i32 blinnPhongUniformGlossiness;
    i32 blinnPhongUniformNormalTexturePresent;

    f32 cameraTargetFOV;
    f32 cameraFOVDelta;
    smath::ivec2 viewport;
    smath::vec2 eulerCameraRotation;
    smath::quat targetCameraRotation;
    smath::vec3 targetCameraPosition;
    Core::camera camera;
    Core::lightBuffer lights;
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


