#include "text.hpp"

using namespace Core;

Text::FontAtlasRange::FontAtlasRange( f32 fontSize, char from, char to ) :
    m_fontSize(fontSize), m_from(from), m_to(to) { }

Text::FontAtlasSettings::FontAtlasSettings( i32 atlasScale ) :
    m_atlasScale(atlasScale) { }

void Text::FontAtlasSettings::AddRange( FontAtlasRange range ) {
    m_ranges.push_back(range);
    m_rangeCount++;
}
