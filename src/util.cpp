/**
 * Description:  Utility functions
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 12, 2022 
 */
#include "util.hpp"
#include "pch.hpp"

void strTowstr( const char* src, wchar_t* dst, usize dstMaxLen ) {
    mbstowcs( &dst[0], src, dstMaxLen );
}

void wstrTostr( const wchar_t* src, char* dst, usize dstMaxLen ) {
    wcstombs( &dst[0], src, dstMaxLen );
}

usize strLen( const char* str ) {
    usize result = 0;
    while(*str++) {
        ++result;
    }
    return result;
}

bool strCmp( const char* strA, const char* strB ) {
    usize strALen = strLen(strA);
    usize strBLen = strLen(strB);
    if( strALen != strBLen ) {
        return false;
    }

    forloop( (isize)strALen ) {
        char charA = strA[i];
        char charB = strB[i];
        if( charA != charB ) {
            return false;
        }
    }

    return true;
}
