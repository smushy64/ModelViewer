#include "renderer.hpp"
#include "gl.hpp"
#include "debug.hpp"

using namespace Platform;

Renderer* Renderer::New( Renderer::Backend backend ) {
    switch (backend) {
        case Renderer::Backend::OPENGL: {
            LOG_INFO("Current Backend: OpenGL Core 4.6");
        } return new RendererOpenGL();
        default: {
            LOG_ERROR("Backend not yet implemented!");
        } return nullptr;
    }
}
