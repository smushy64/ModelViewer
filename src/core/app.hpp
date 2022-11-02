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

namespace UI {
    class Canvas;
}

struct SettingsINI {
    i32 width, height;
    Platform::BackendAPI backend;
};
SettingsINI DefaultSettings();
bool ParseSettings( const std::string& text, Core::SettingsINI& result );

struct AppData {
    bool isRunning;
    f32 deltaTime;
    Platform::Renderer* renderer;
    Core::UI::Canvas canvas;
    glm::vec2 screenResolution;
    Input input;
    Core::EventQueue events;
};

const inline f32 CAMERA_ZOOM_FACTOR = 3.5f;

void RunApp( AppData& data );

} // namespace Core
