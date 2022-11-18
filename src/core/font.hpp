/**
 * Description:  Font Atlas
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 16, 2022 
*/
#pragma once
#include "pch.hpp"

// forward declaration
namespace Platform {
    struct File;
};

namespace Core {

struct FontMetrics {
    /// character coordinates on atlas texture
    f32 atlasX, atlasY;
    /// character scale on atlas texture
    f32 atlasW, atlasH;

    /// bearings in screen pixels
    i32 leftBearing, topBearing;
    /// character advance in screen pixels
    i32 advance;
    /// character scale
    f32 width, height;
};

/// font key value pair
struct FontMetricsKV {
    char key;
    FontMetrics metrics;
};

/// font map
struct FontMap {
    usize count;
    FontMetricsKV* map;
    /// @brief Retrieve a FontMetrics struct from the map
    /// @param key key to search for
    /// @return pointer to font metrics, null if not found
    FontMetrics* get( char key );
};

struct FontAtlas {
    f32     pointSize;
    i32     width, height;
    usize   charCount;
    usize   fontNameLen;
    char*   fontName;
    FontMap metrics;
    u8*     bitmap;
};

/// @brief Create a font atlas
/// @param fontFile file to load font from
/// @param pointSize character point size
/// @param atlasW width of atlas texture
/// @param atlasH height of atlas texture
/// @param from,to range of characters to load
/// @param result [out] font atlas result
/// @return true if successful
bool CreateFontAtlas(
    Platform::File* fontFile,
    f32 pointSize,
    i32 atlasW, i32 atlasH,
    char from, char to,
    FontAtlas* result
);

/// @brief Free font atlas memory
void FreeFontAtlas( FontAtlas* fontAtlas );

} // namespace Core

