#pragma once
#include <string>
#include <map>
#include "defines.hpp"

namespace Core
{

namespace Font
{

    struct AtlasChar {
        // coordinate on atlas texture
        f32 atlasX, atlasY;
        // scale on atlas texture
        f32 atlasWidth, atlasHeight;
        
        // VVVVV IMPORTANT: in pixels!!! VVVVVV
        
        // left side bearing in screen pixels
        i32 leftBearing;
        // top side bearing in screen pixels
        i32 topBearing;
        // x advance in screen pixels
        i32 advance;
        // width in screen pixels
        f32 width; // character width
        // height in screen pixels ?
        f32 height; // character height
    };

    struct Atlas {
        // name of font
        std::string fontName;
        // Metrics for each character
        std::map<char, AtlasChar> charMetrics;
        // character count in atlas
        usize characterCount;
        // atlas point size for font
        f32 pointSize;
        // uniform scaling on x and y so only one value is needed
        i32 scale;
        // renderer texture id
        RendererID textureID;
        // IMPORTANT: Only used to load into renderer so most of the time this will be null!
        u8* bitmap;
    };

    bool LoadFontAtlas( const char* fontPath, f32 pointSize, char fromChar, char toChar, i32 atlasScale, Atlas& result );
    void FreeAtlasBitmap( Atlas& atlas );

} // namespace Core::Font

} // namespace Core

