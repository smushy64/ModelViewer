#pragma once
#include "platform/renderer.hpp"
#include "alias.hpp"
#include "event.hpp"
#include "input.hpp"
#include <queue>

namespace Core {

struct AppData {
    bool isRunning;
    f32 deltaTime;
    Platform::Renderer* renderer;
    glm::vec2 screenResolution;
    Input input;
    EventQueue events;
};

void RunApp( AppData& data );

} // namespace Core
