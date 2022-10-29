#include "settings.hpp"
#include "consts.hpp"

Core::SettingsINI Core::DefaultSettings() {
    Core::SettingsINI result = {};
    result.width   = DEFAULT_WINDOW_WIDTH;
    result.height  = DEFAULT_WINDOW_HEIGHT;
    result.backend = Platform::BackendAPI::OPENGL;
    return result;
}
