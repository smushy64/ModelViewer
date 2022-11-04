#pragma once
#include "pch.hpp"
#include "mousecode.hpp"
#include "keycode.hpp"

namespace Core {

struct Input {
    i32  mouseScroll;
    glm::vec2  screenSpaceMouse;
    glm::ivec2 pixelMouse;
    glm::vec2  lastScreenSpaceMouse;
    glm::ivec2 lastPixelMouse;
    glm::ivec2 pixelMouseDelta;
    glm::vec2  screenSpaceMouseDelta;
    bool mouseButtons[(usize)MouseCode::count];
    bool keys[(usize)KeyCode::count];
};

} // namespace Core

