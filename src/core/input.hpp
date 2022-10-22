#pragma once
#include "alias.hpp"
#include "mousecode.hpp"
#include "keycode.hpp"
#include <glm/vec2.hpp>

namespace Core {

struct Input {
    i32  mouseScroll;
    glm::vec2  screenSpaceMouse;
    glm::ivec2 pixelMouse;
    bool mouseButtons[(usize)MouseCode::count];
    bool keys[(usize)KeyCode::count];
};

} // namespace Core

