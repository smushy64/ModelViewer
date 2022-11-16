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

usize strLenW( const wchar_t* str ) {
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

bool strCmpW( const wchar_t* strA, const wchar_t* strB ) {
    usize strALen = strLenW(strA);
    usize strBLen = strLenW(strB);
    if( strALen != strBLen ) {
        return false;
    }

    forloop( (isize)strALen ) {
        wchar_t charA = strA[i];
        wchar_t charB = strB[i];
        if( charA != charB ) {
            return false;
        }
    }

    return true;
}

void strConcat(
    usize srcASize, const char* srcA,
    usize srcBSize, const char* srcB,
    usize dstSize, char* dst
) {
    // TODO(alicia): dst bounds checking!!!
    UNUSED_PARAM(dstSize);
    forloop( (isize)srcASize ) {
        *dst++ = *srcA++;
    }
    forloop( (isize)srcBSize ) {
        *dst++ = *srcB++;
    }
    *dst++ = 0;
}

void strConcatW(
    usize srcASize, const wchar_t* srcA,
    usize srcBSize, const wchar_t* srcB,
    usize dstSize, wchar_t* dst
) {
    // TODO(alicia): dst bounds checking!!!
    UNUSED_PARAM(dstSize);
    forloop( (isize)srcASize ) {
        *dst++ = *srcA++;
    }
    forloop( (isize)srcBSize ) {
        *dst++ = *srcB++;
    }
    *dst++ = 0;
}
