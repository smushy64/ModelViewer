#pragma once

#include <string>

namespace Utils {
    std::wstring CStringToWString( const char* cstring );
    void EraseSubString( std::string& string, const std::string& subString );
    std::wstring StringtoWString( const std::string& string );
    std::string WStringToString( const std::wstring& wstring );
} // namespace Utils

