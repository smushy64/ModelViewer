#include "font.hpp"
#include "debug.hpp"
#include "platform/io.hpp"

#include "platform/utils.hpp"
#include <sstream>
#include <vector>
// TODO: rewrite this to be more sensible
std::string FormatFontName( const char* fontPath ) {
    std::stringstream stream(fontPath);
    std::string segment;
    std::vector<std::string> seglist;
    
    while(std::getline( stream, segment, '/' )) {
        seglist.push_back( segment );
    }

    std::string result = seglist.back();

    // NOTE: Why is this under platform?????
    Platform::Utils::EraseSubString( result, ".ttf" );
    Platform::Utils::EraseSubString( result, ".otf" );

    return result;
}

#define STB_RECT_PACK_IMPLEMENTATION 1
#include "stb/stb_rect_pack.h"

#define STB_TRUETYPE_IMPLEMENTATION 1
#include "stb/stb_truetype.h"

bool Core::Font::LoadFontAtlas(
    const char* fontPath,
    f32 pointSize,
    char fromChar,
    char toChar,
    i32 atlasScale,
    Atlas& result
) {

    DEBUG_ASSERT(toChar > fromChar);

    auto font = Platform::IO::LoadFile( fontPath );
    if( !font.contents ) {
        return false;
    }

    // zero out struct
    result = {};
    result.fontName  = FormatFontName( fontPath );
    result.scale     = atlasScale;
    result.pointSize = pointSize;
    result.bitmap = (u8*)malloc( result.scale * result.scale );
    if( !result.bitmap ) {
        LOG_ERROR("Failed to malloc space for Font Atlas Bitmap!");
        Platform::IO::FreeFile( font );
        return false;
    }

    stbtt_pack_context packContext;
    if( !stbtt_PackBegin(
        &packContext,
        result.bitmap,
        result.scale,
        result.scale,
        0, // stride in bytes ?
        1, // padding ?
        nullptr // alloc context ?
    ) ) {
        LOG_ERROR("Failed to begin packing Font Atlas!");
        free(result.bitmap);
        result = {};
        Platform::IO::FreeFile( font );
        return -1;
    }

    stbtt_PackSetOversampling( &packContext, 1, 1 );

    usize glyphCount = (usize)toChar - (usize)fromChar + 1;
    stbtt_packedchar glyphMetrics[glyphCount];
    stbtt_pack_range range = {};
    range.font_size                        = result.pointSize;
    range.first_unicode_codepoint_in_range = fromChar;
    range.array_of_unicode_codepoints      = nullptr;
    range.num_chars                        = (i32)glyphCount;
    range.chardata_for_range               = glyphMetrics;

    if(!stbtt_PackFontRanges(
        &packContext,
        (u8*)font.contents,
        0, // font index
        &range,
        1 // number of ranges
    )) {
        LOG_ERROR("Failed to pack font ranges!");
        free(result.bitmap);
        result = {};
        Platform::IO::FreeFile( font );
        return -1;
    }
    stbtt_PackEnd(&packContext);

    stbtt_fontinfo fontInfo;
    stbtt_InitFont(
        &fontInfo,
        (u8*)font.contents,
        stbtt_GetFontOffsetForIndex( (u8*)font.contents, 0 )
    );

    f32 em = stbtt_ScaleForMappingEmToPixels(
        &fontInfo,
        result.pointSize
    );

    for( usize i = 0; i < glyphCount; i++ ) {
        char character = fromChar + (char)i;
        AtlasChar atlasChar = {};
        const stbtt_packedchar& metric = glyphMetrics[i];

        // IMPORTANT: As far as I can tell, this is all correct :|
        atlasChar.width  = (f32)(metric.x1 - metric.x0);
        atlasChar.height = (f32)(metric.y1 - metric.y0);
        atlasChar.atlasX      = (f32)(metric.x0) / (f32)result.scale;
        atlasChar.atlasY      = (f32)(metric.y0) / (f32)result.scale;
        atlasChar.atlasWidth  = atlasChar.width  / (f32)result.scale;
        atlasChar.atlasHeight = atlasChar.height / (f32)result.scale;

        // IMPORTANT: Still not sure about this stuff
        i32 advanceWidth, leftSideBearing;
        stbtt_GetCodepointHMetrics(
            &fontInfo,
            character,
            &advanceWidth,
            &leftSideBearing
        );

        atlasChar.leftBearing = leftSideBearing * em;
        atlasChar.topBearing  = metric.yoff2;
        atlasChar.advance     = metric.xadvance;

        result.charMetrics.insert( std::pair< char, AtlasChar >(
            character,
            atlasChar
        ) );
        result.characterCount++;
    }

    Platform::IO::FreeFile( font );
    return true;
}

void Core::Font::FreeAtlasBitmap( Atlas& atlas ) {
    LOG_DEBUG("Freed bitmap from font \"%s\"!", atlas.fontName.c_str());
    free( atlas.bitmap );
}
