#include "utils.hpp"
#include "alias.hpp"
#include "debug.hpp"

std::wstring Utils::CStringToWString( const char* cstring ) {
    isize strLen = mbstowcs( nullptr, cstring, 0 );
    if( strLen == -1 ) {
        LOG_ERROR("Failed to convert const char* to std::wstring. Invalid source str!");
        return std::wstring();
    }

    std::wstring wstring( strLen, 0 );
    mbstowcs( &wstring[0], cstring, wstring.size() );

    return wstring;
}

void Utils::EraseSubString( std::string& string, const std::string& subString ) {
    usize position = string.find(subString);
    if( position != std::string::npos ) {
        string.erase( position, subString.length() );
    }
}

std::wstring Utils::StringtoWString( std::string const& string ) {
    isize strLen = mbstowcs( nullptr, &string[0], 0 );
    if( strLen == -1 ) {
        LOG_ERROR("Failed to convert std::string to std::wstring. Invalid source str!");
        return std::wstring();
    }

    std::wstring wstring( strLen, 0 );
    mbstowcs( &wstring[0], &string[0], wstring.size() );

    return wstring;
}
