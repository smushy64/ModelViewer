/**
 * Description:  Image Loading
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 22, 2022 
*/
#include "core/image.hpp"
#include "platform/io.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define COLLECT_ALL_CHANNELS 0

bool Core::ReadImage( usize filesize, void* file, Image* result ) {

    stbi_set_flip_vertically_on_load( true );
    result->data = stbi_load_from_memory(
        (u8*)file,
        filesize,
        &result->width,
        &result->height,
        &result->colorComponentCount,
        COLLECT_ALL_CHANNELS
    );

    if( !result->data ) {
        LOG_WARN("ReadImage > Failed to read image!");
        *result = {};
        return false;
    }

    result->totalSize = (usize)(result->width * result->height * result->colorComponentCount);

    return true;

}
void Core::FreeImage( Image* image ) {
    if( image->data ) {
        stbi_image_free( image->data );
    }
}
