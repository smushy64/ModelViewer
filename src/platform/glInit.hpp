#pragma once

namespace Platform {
typedef void (*OpenGLSwapBuffer)(void);
typedef void* (*OpenGLLoader)(const char* functionName);

struct OpenGLInitData {
    OpenGLSwapBuffer swapBufferFn;
    OpenGLLoader     loaderFn;
};
} // namespace Platform

