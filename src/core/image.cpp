#include "image.hpp"
#include "debug.hpp"

#define STB_IMAGE_IMPLEMENTATION 1
#include "include/stb/stb_image.h"

Core::Image Core::ReadImage( void* data, usize size ) {
    Image result = {};
    result.size = size;

    i32 channelsInFile = 0;
    const i32 DESIRED_CHANNELS = 0; // auto

    stbi_set_flip_vertically_on_load(1);
    result.bitmap = stbi_load_from_memory(
        (u8*)data,
        size,
        &result.dimensions.x,
        &result.dimensions.y,
        &channelsInFile,
        DESIRED_CHANNELS
    );

    result.format = (ImageFormat)channelsInFile;

    return result;
}

void Core::FreeImage( Image image ) {
    if( image.bitmap ) {
        stbi_image_free( image.bitmap );
    }
    image.size = 0;
}
