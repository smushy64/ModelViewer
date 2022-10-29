#pragma once
#include "platform/backend.hpp"
#include "settings.hpp"

#include <string>

namespace Core {
    bool ParseSettings( const std::string& text, Core::SettingsINI& result );
} // namespace Core

