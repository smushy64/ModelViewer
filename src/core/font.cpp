/**
 * Description:  Font Atlas
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 16, 2022 
*/
#include "core/font.hpp"
#include "platform/io.hpp"
#include "util.hpp"

#define STB_RECT_PACK_IMPLEMENTATION 1
#include "stb/stb_rect_pack.h"

#define STB_TRUETYPE_IMPLEMENTATION 1
#include "stb/stb_truetype.h"

#define PACKED_TIGHT 0
#define PADDING 1
#define VOVERSAMPLE 1
#define HOVERSAMPLE 1
#define MAX_FONT_NAME_LEN 128ULL

// TODO(alicia): define own malloc/free for stb use?

void ProcessFilePathToFontName( usize filePathLen, const char* filePath, usize maxFontNameLen, char* fontName ) {
    // get last / position
    usize outPos = 0;
    usize lastSlashPos = 0;
    usize count = 0;
    while( findChar( &filePath[lastSlashPos+count], '/', &outPos ) ) {
        lastSlashPos += outPos;
        count++;
    }
    lastSlashPos += count;

    // cut out path up to stem
    usize fileNameLastPathLen = (filePathLen - lastSlashPos);
    ucycles(fileNameLastPathLen) {
        if(i == maxFontNameLen) {
            break;
        }
        fontName[i] = filePath[i + lastSlashPos];
    }
    if( fileNameLastPathLen > maxFontNameLen ) {
        fontName[maxFontNameLen-1] = '\0';
    } else {
        fontName[fileNameLastPathLen] = '\0';
    }
}

bool Core::CreateFontAtlas(
    Platform::File* fontFile,
    f32 pointSize,
    i32 atlasW, i32 atlasH,
    char from, char to,
    FontAtlas* result
) {
    DEBUG_ASSERT_LOG( to > from,
        "Create Font Atlas > Invalid character range! %c -> %c",
        from, to
    );
    DEBUG_ASSERT_LOG( fontFile->data,
        "Create Font Atlas > Font file contents are null!"
    );

    result->fontName = (char*)Platform::Alloc(MAX_FONT_NAME_LEN);
    if( !result->fontName ) {
        LOG_ERROR("Create Font Atlas > Failed to allocate %llu bytes for fontName!", MAX_FONT_NAME_LEN);
        return false;
    }
    result->width     = atlasW;
    result->height    = atlasH;
    usize bitmapSize  = result->width * result->height;
    result->bitmap    = (u8*)Platform::Alloc( bitmapSize );
    if( !result->bitmap ) {
        result = {};
        LOG_ERROR( "Create Font Atlas > Failed to allocate %llu bytes for atlas bitmap!", bitmapSize );
        return false;
    }
    result->fontNameLen = strLen( result->fontName );
    result->pointSize = pointSize;
    
    ProcessFilePathToFontName(
        fontFile->filePathLen,
        fontFile->filePath,
        MAX_FONT_NAME_LEN,
        result->fontName
    );

    stbtt_pack_context packContext;
    if( !stbtt_PackBegin(
        &packContext,
        result->bitmap,
        result->width,
        result->height,
        PACKED_TIGHT,
        PADDING,
        nullptr // alloc context ?
    ) ) {
        LOG_ERROR( "Create Font Atlas > Failed to begin packing font atlas!" );
        Platform::Free( result->bitmap );
        result = {};
        return false;
    }

    stbtt_PackSetOversampling( &packContext, HOVERSAMPLE, VOVERSAMPLE );
    usize glyphCount = ((usize)to - (usize)from) + 1;
    result->metrics.count = glyphCount;
    result->metrics.map   = (FontMetricsKV*)Platform::Alloc( glyphCount * sizeof( FontMetricsKV ) );
    if( !result->metrics.map ) {
        LOG_ERROR( "Create Font Atlas > Failed to allocate %llu bytes for atlas character map!", glyphCount );
        stbtt_PackEnd( &packContext );
        Platform::Free( result->bitmap );
        result = {};
        return false;
    }

    stbtt_packedchar glyphMetrics[glyphCount];

    const usize numberOfRanges = 1;
    stbtt_pack_range packRange                 = {};
    packRange.font_size                        = result->pointSize;
    packRange.first_unicode_codepoint_in_range = from;
    packRange.num_chars                        = smath::clampTrunc64to32((isize)glyphCount);
    packRange.chardata_for_range               = glyphMetrics;

    u8* fontPtr = (u8*)fontFile->data;
    bool packResult = stbtt_PackFontRanges(
        &packContext,
        fontPtr,
        0, // font index
        &packRange,
        numberOfRanges
    );
    stbtt_PackEnd( &packContext );

    if(!packResult) {
        LOG_ERROR( "Create Font Atlas > Failed to pack font ranges!" );
        Platform::Free( result->bitmap );
        Platform::Free( result->metrics.map );
        result = {};
        return false;
    }

    stbtt_fontinfo fontInfo;
    // NOTE(alicia): InitFont does not have a corresponding "EndFont" or something like that
    if(!stbtt_InitFont(
        &fontInfo,
        fontPtr,
        stbtt_GetFontOffsetForIndex( fontPtr, 0 )
    )) {
        LOG_ERROR( "Create Font Atlas > Failed to initialize font!" );
        Platform::Free( result->bitmap );
        Platform::Free( result->metrics.map );
        result = {};
        return false;
    }

    // TODO(alicia): find out why this is needed
    f32 emScale = stbtt_ScaleForMappingEmToPixels( &fontInfo, result->pointSize );

    cycles( (isize)glyphCount ) {
        char character = from + (char)smath::clampTrunc64to8(i);
        FontMetrics metrics = {};
        stbtt_packedchar packedChar = glyphMetrics[i];

        metrics.width  = (f32)( packedChar.x1 - packedChar.x0 );
        metrics.height = (f32)( packedChar.y1 - packedChar.y0 );
        metrics.atlasX = (f32)( packedChar.x0 ) / (f32)result->width;
        metrics.atlasY = (f32)( packedChar.y0 ) / (f32)result->height;
        metrics.atlasW = metrics.width  / (f32)result->width;
        metrics.atlasH = metrics.height / (f32)result->height;

        // TODO(alicia): figure this stuff out
        i32 advanceWidth, leftSideBearing;
        stbtt_GetCodepointHMetrics(
            &fontInfo,
            character,
            &advanceWidth,
            &leftSideBearing
        );

        metrics.leftBearing = (i32)((f32)leftSideBearing * emScale);
        metrics.topBearing  = packedChar.yoff2;
        metrics.advance     = packedChar.xadvance;

        result->metrics.map[i].key     = character;
        result->metrics.map[i].metrics = metrics;
    }

    return true;
}

bool Core::FontMap::get( char key, FontMetrics* result ) {
    result = {};
    ucycles( count ) {
        if( map[i].key == key ) {
            result = &map[i].metrics;
            return true;
        }
    }
    LOG_WARN("Font Map > Character \'%c\' not found!", key);
    return false;
}

void Core::FreeFontAtlas( FontAtlas* fontAtlas ) {
    if( fontAtlas->bitmap ) {
        Platform::Free( fontAtlas->bitmap );
    }
    Platform::Free(fontAtlas->fontName);
}
