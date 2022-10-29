#pragma once
#include "platform/renderer.hpp"
#include "alias.hpp"
#include "event.hpp"
#include "input.hpp"
#include "light.hpp"
#include <queue>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Core {

struct AppData {
    bool isRunning;
    f32 deltaTime;
    Platform::Renderer* renderer;
    Platform::VertexArray* va;
    Platform::Texture2D* albedo;
    Platform::Texture2D* specular;
    Core::Camera* camera;
    Core::Lights* lights;
    Platform::Shader* sh;
    glm::vec2 screenResolution;
    Input input;
    Core::EventQueue events;
};

const inline f32 CAMERA_ZOOM_FACTOR = 3.5f;

void RunApp( AppData& data );

} // namespace Core
