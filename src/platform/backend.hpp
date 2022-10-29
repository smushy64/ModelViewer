#pragma once

namespace Platform {
    enum class BackendAPI {
        OPENGL
    };
    const char* BackendToString( BackendAPI backend );
}