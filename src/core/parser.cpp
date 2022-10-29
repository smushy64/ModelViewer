#include "parser.hpp"
#include "debug.hpp"
#include <sstream>
#include <vector>
#include "platform/backend.hpp"

// Maybe rework this later? Works well enough as it is though

enum class Mode {
    NONE,
    GRAPHICS,
    RESOLUTION
};
bool ParseMode( const std::string& token, Mode& result ) {
    if( token == "[GRAPHICS]" ) {
        result = Mode::GRAPHICS;
        return true;
    } else if( token == "[RESOLUTION]" ) {
        result = Mode::RESOLUTION;
        return true;
    }

    return false;
}

enum class ResolutionMode {
    NONE,
    WIDTH,
    HEIGHT
};
bool ParseResolutionMode( const std::string& token, ResolutionMode& result ) {
    if( token == "width" ) {
        result = ResolutionMode::WIDTH;
    } else if( token == "height" ) {
        result = ResolutionMode::HEIGHT;
    } else {
        return false;
    }
    return true;
}

bool ParseBackend( const std::string& string, Platform::BackendAPI& result ) {
    if(
        string == "OPENGL" ||
        string == "OpenGL" ||
        string == "OpenGL Core 4.6"
    ) {
        result = Platform::BackendAPI::OPENGL;
    }
    else {
        return false;
    }
    return true;
}

void ParseToken(
    const std::string& token,
    Core::SettingsINI& result,
    Mode& mode,
    ResolutionMode& resolutionMode
) {
    if( ParseMode( token, mode ) ) {
        return;
    }

    switch( mode ) {
        case Mode::GRAPHICS: {
            Platform::BackendAPI api;
            if( ParseBackend( token, api ) ) {
                LOG_INFO("Settings Parser > API: %s", Platform::BackendToString(api));
                result.backend = api;
            }
        } break;
        case Mode::RESOLUTION: {
            ParseResolutionMode( token, resolutionMode );
            if( resolutionMode != ResolutionMode::NONE ) {
                i32 number;
                bool success = true;
                try {
                    number = std::stoi( token );
                } catch( std::invalid_argument const& ex ) { success = false; }
                catch( std::out_of_range const& ex ) { success = false; }

                if( success ) {
                    if( resolutionMode == ResolutionMode::WIDTH ) {
                        LOG_INFO("Settings Parser > Width: %i", number);
                        result.width = number;
                        resolutionMode = ResolutionMode::NONE;
                    } else if( resolutionMode == ResolutionMode::HEIGHT ) {
                        LOG_INFO("Settings Parser > Height: %i", number);
                        result.height = number;
                        resolutionMode = ResolutionMode::NONE;
                    }
                }
            }
        } break;
        default: return;
    }
}

bool Core::ParseSettings( const std::string& text, Core::SettingsINI& result ) {

    std::stringstream ss = std::stringstream(text);
    std::string line;
    Mode currentMode = Mode::NONE;
    ResolutionMode resolutionMode = ResolutionMode::NONE;
    while( std::getline( ss, line, '\n' ) ) {
        // skip comments
        if( line[0] == ';' ) {
            continue;
        }

        std::stringstream innerSS = std::stringstream(line);
        std::string token;
        while( std::getline( innerSS, token, ' ' ) ) {
            ParseToken( token, result, currentMode, resolutionMode );
        }

    }

    return true;
}
