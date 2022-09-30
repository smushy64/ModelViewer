#pragma once
#include "defines.hpp"
#include <vector>
#include <map>
#include <string>

namespace Core
{

namespace Text
{
    
enum class AnchorHorizontal { LEFT, CENTER, RIGHT };
enum class AnchorVertical   { TOP, CENTER, BOTTOM };

class FontAtlasRange {
public:
    FontAtlasRange( f32 fontSize, char from, char to );
    f32  FontSize() const { return m_fontSize; }
    char FromChar() const { return m_from; }
    char ToChar()   const { return m_to; }
private:
    f32 m_fontSize;
    char m_from, m_to;
};

class FontAtlasSettings {
public:
    FontAtlasSettings( i32 atlasScale );
    void AddRange( FontAtlasRange range );
    
    const FontAtlasRange& GetRange( usize i ) const { return m_ranges[i]; }
    i32 GetScale()                            const { return m_atlasScale; }
    usize RangeCount()                        const { return m_rangeCount; }
private:
    i32 m_atlasScale;
    usize m_rangeCount = 0;
    std::vector<FontAtlasRange> m_ranges;
};

struct AtlasSettings {
    i32 scaleX, scaleY;
    char fromChar, toChar;
    f32 fontSize;
};

struct CharMetrics {
    f32 texCoordX, texCoordY; // coordinate on atlas
    f32 texWidth,  texHeight; // scale on atlas
    
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
struct FontAtlas {
    std::string fontName;
    std::map<char, CharMetrics> characters;
    usize characterCount;
    f32 fontSize;
    i32 scaleX, scaleY;   // atlas scale
    RendererID textureID; // atlas texture id
};

} // namespace Core::Text

} // namespace Core


