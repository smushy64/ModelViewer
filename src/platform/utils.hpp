#pragma once
#include <string>

namespace Platform {

namespace Utils
{
    
    std::wstring CStringToWString( const char* cstring );
    std::wstring StringtoWString( std::string const& string );

    void EraseSubString( std::string& string, const std::string& subString );

} // namespace Platform::Utils

} // namespace Platform
