#pragma once

namespace Platform {
    enum class BackendAPI {
        OPENGL,
        DIRECTX11,
    };
    const char* BackendToString( BackendAPI backend );
}