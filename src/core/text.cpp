#include "text.hpp"
#include "debug.hpp"
#include <sstream>
#include <vector>
#include "utils.hpp"
#include "consts.hpp"

#define STB_RECT_PACK_IMPLEMENTATION 1
#include "stb/stb_rect_pack.h"

#define STB_TRUETYPE_IMPLEMENTATION 1
#include "stb/stb_truetype.h"

// TODO: rewrite this to be more sensible
std::string FormatFontName( const char* fontPath ) {
    std::stringstream stream(fontPath);
    std::string segment;
    std::vector<std::string> seglist;
    
    while(std::getline( stream, segment, '/' )) {
        seglist.push_back( segment );
    }

    std::string result = seglist.back();

    Utils::EraseSubString( result, ".ttf" );
    Utils::EraseSubString( result, ".otf" );

    return result;
}

const i32 DEFAULT_ATLAS_SCALE = 512;
bool Core::CreateFontAtlas(
    const Platform::File& fontFile,
    const FontAtlasSettings& settings,
    FontAtlas& result
) {

    DEBUG_ASSERT_LOG(settings.toChar > settings.fromChar,
        "Create Font Atlas > Invalid character range! %c -> %c",
        settings.fromChar, settings.toChar
    );

    DEBUG_ASSERT_LOG(fontFile.contents, "Create Font Atlas > File contents are null!");

    // zero result
    result = {};
    result.name = FormatFontName( fontFile.filePath );
    result.textureID = U32_MAX; // max u32 so that i know that it's uninitialized
    if( settings.atlasScale == 0 ) {
        result.atlasScale = DEFAULT_ATLAS_SCALE;
    } else {
        result.atlasScale = settings.atlasScale;
    }
    result.pointSize = settings.pointSize;
    result.bitmap    = (u8*)malloc( result.atlasScale * result.atlasScale );
    if( !result.bitmap ) {
        LOG_ERROR( "Create Font Atlas > Failed to allocate space for Font Atlas Bitmap!" );
        return false;
    }

    stbtt_pack_context packContext;
    if( !stbtt_PackBegin(
        &packContext,
        result.bitmap,
        result.atlasScale,
        result.atlasScale,
        0, // stride in bytes ?
        1, // padding ?
        nullptr // alloc context ?
    ) ) {
        LOG_ERROR("Create Font Atlas > Failed to begin packing Font Atlas!");
        free(result.bitmap);
        result = {};
        return false;
    }

    // IMPORTANT: find out what this does, I just know it's necessary
    stbtt_PackSetOversampling( &packContext, 1, 1 );

    usize glyphCount = ((usize)settings.toChar - (usize)settings.fromChar) + 1;
    stbtt_packedchar glyphMetrics[glyphCount];
    stbtt_pack_range range = {};
    range.font_size                        = result.pointSize;
    range.first_unicode_codepoint_in_range = settings.fromChar;
    range.array_of_unicode_codepoints      = nullptr;
    // NOTE: handle usize being larger than i32 size?
    range.num_chars                        = (i32)glyphCount;
    range.chardata_for_range               = glyphMetrics;

    // byte ptr to font file contents
    u8* fontPtr = (u8*)fontFile.contents;
    if(!stbtt_PackFontRanges(
        &packContext,
        fontPtr,
        0, // font index
        &range,
        1 // number of ranges
    )) {
        LOG_ERROR("Create Font Atlas > Failed to pack font ranges!");
        free(result.bitmap);
        result = {};
        return false;
    }
    stbtt_PackEnd(&packContext);

    stbtt_fontinfo fontInfo;
    stbtt_InitFont(
        &fontInfo,
        fontPtr,
        stbtt_GetFontOffsetForIndex( fontPtr, 0 )
    );

    // IMPORTANT: Get em scale, not entirely sure what's going on here but it works?
    f32 em = stbtt_ScaleForMappingEmToPixels(
        &fontInfo,
        result.pointSize
    );

    for( usize i = 0; i < glyphCount; i++ ) {
        char character = settings.fromChar + (char)i;
        CharMetrics atlasChar = {};
        const stbtt_packedchar& metric = glyphMetrics[i];

        // IMPORTANT: As far as I can tell, this is all correct :|
        atlasChar.width       = (f32)(metric.x1 - metric.x0);
        atlasChar.height      = (f32)(metric.y1 - metric.y0);
        atlasChar.atlasX      = (f32)(metric.x0) / (f32)result.atlasScale;
        atlasChar.atlasY      = (f32)(metric.y0) / (f32)result.atlasScale;
        atlasChar.atlasWidth  = atlasChar.width  / (f32)result.atlasScale;
        atlasChar.atlasHeight = atlasChar.height / (f32)result.atlasScale;

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

        result.characterMetrics.insert( std::pair< char, CharMetrics >(
            character,
            atlasChar
        ) );
        result.characterCount++;
    }

    return true;

}
