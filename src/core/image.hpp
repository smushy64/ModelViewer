#pragma once
#include "alias.hpp"
#include <glm/vec2.hpp>

namespace Core {

enum class ImageFormat {
    UNKNOWN = 0,
    R,
    RG,
    RGB,
    RGBA
};

struct Image {
    ImageFormat format;
    usize size;
    glm::ivec2 dimensions;
    u8* bitmap;
};

Image ReadImage( void* data, usize size );
void FreeImage( Image image );

} // namespace Core

