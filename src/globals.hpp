#pragma once

#include "platform/renderer.hpp"

bool g_PROGRAM_RUNNING = false;
Platform::Renderer::API::Backend g_CURRENT_BACKEND = Platform::Renderer::API::Backend::OPENGL;
