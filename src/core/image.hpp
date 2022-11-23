/**
 * Description:  Image Loading
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 22, 2022 
*/
#pragma once
#include "pch.hpp"

namespace Core {
    
struct Image {
    i32 width, height;
    i32 colorComponentCount;
    usize totalSize;
    u8* data;
};

/// @brief read image from file
/// @param filesize byte size of file
/// @param file file data
/// @param result result
/// @return true if successful
bool ReadImage( usize filesize, void* file, Image* result );

void FreeImage( Image* image );

} // namespace Core

