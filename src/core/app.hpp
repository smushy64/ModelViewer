#pragma once

#include "platform/renderer.hpp"

namespace Core {

struct AppData {
    Platform::Renderer* renderer;
};

void RunApp( AppData data );

} // namespace Core
