#pragma once
#include "alias.hpp"
#include "platform/backend.hpp"

namespace Core {

struct SettingsINI {
    i32 width, height;
    Platform::BackendAPI backend;
};

SettingsINI DefaultSettings();

} // namespace Core
