#pragma once
#include "pch.hpp"
#include "platform/io.hpp"
#include <map>

namespace Platform {
    class Texture2D;
}

namespace Core
{

// Text anchor on x axis
enum class XAnchor {
    LEFT,
    CENTER,
    RIGHT
};
// Text anchor on y axis
enum class YAnchor {
    TOP,
    CENTER,
    BOTTOM
};

inline XAnchor DEFAULT_X_ANCHOR = XAnchor::LEFT;
inline YAnchor DEFAULT_Y_ANCHOR = YAnchor::BOTTOM;

// Atlas Character Metrics
struct CharMetrics {
    // Character coordinate on atlas texture
    f32 atlasX, atlasY;
    // Character scale on atlas texture
    f32 atlasWidth, atlasHeight;

    // bearing in screen pixels
    i32 leftBearing, topBearing;
    // advance in screen pixels
    i32 advance;
    // scale in screen pixels
    f32 width, height;
};

// Font Atlas, texture containing a packed set of characters for rendering
struct FontAtlas {
    // Point size of font on atlas
    f32 pointSize;
    // Pixel scale of atlas, uniform on x and y axis
    i32 atlasScale;
    // Number of characters on atlas
    usize characterCount;
    // Name of font
    std::string name;
    // Metrics of each character on atlas
    std::map<char, CharMetrics> characterMetrics;
    // Pointer to bitmap, set to null when bitmap is uploaded to GPU
    u8* bitmap;
    // GPU atlas texture
    Platform::Texture2D* texture;
};

// Settings for creating a Font Atlas
struct FontAtlasSettings {
    // Point size of font on atlas
    f32 pointSize;
    // Pixel scale of atlas, uniform on x and y axis
    // If 0, uses default scale (512)
    i32 atlasScale;
    // Character range to load
    char fromChar, toChar;
};

bool CreateFontAtlas( const Platform::File& fontFile, const FontAtlasSettings& settings, FontAtlas& result );

} // namespace Core
