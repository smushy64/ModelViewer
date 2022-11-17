/**
 * Description:  Utility functions
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 12, 2022 
 */
#include "util.hpp"
#include "pch.hpp"
#include "platform/io.hpp"

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

bool strCmp( usize strALen, const char* strA, usize strBLen, const char* strB ) {
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

bool getSubStrPos( usize strLen, const char* str, usize subStrLen, const char* subStr, usize* pos ) {
    usize counter = 1;
    while( *str++ ) {
        if( *str == subStr[0] ) {
            if( strLen - counter < subStrLen ) {
                return false;
            }
            if(strCmp( subStrLen, str, subStrLen, subStr )) {
                *pos = counter;
                return true;
            }
        } else {
            ++counter;
        }
    }
    return false;
}

void strClearSubStr(
    usize srcLen, const char* src,
    usize subStrLen, const char* subStr,
    usize maxDstLen, char* dst
) {
    usize subStrPos = 0;
    if( !getSubStrPos(
        srcLen, src,
        subStrLen, subStr,
        &subStrPos
    ) ) {
        LOG_WARN("Clear Substring > Substring \"%s\" was not found in source string \"%s\"!",
            subStr, src
        );
        return;
    }

    usize pos = 0;
    usize charsWritten = 0;
    do {
        if( pos < subStrPos || pos >= subStrPos + subStrLen ) {
            *dst++ = *src;
            charsWritten++;
        }
        pos++;
        if( charsWritten == maxDstLen ) {
            break;
        }
    } while( *src++ );
}

void strInit( usize len, char* buffer ) {
    ucycles( len ) {
        buffer[i] = ' ';
    }
    buffer[len] = '\0';
}

bool isCharPresent( const char* str, char toFind ) {
    do {
        if( *str == toFind ) {
            return true;
        }
    } while( *str++ );
    return false;
}

bool findChar( const char* str, char toFind, usize* pos ) {
    usize position = 0;
    do {
        if( *str == toFind ) {
            *pos = position;
            return true;
        }
        position++;
    } while( *str++ );
    return false;
}

void strCopy( const char* src, usize maxDstLen, char* dst ) {
    ucycles( maxDstLen ) {
        dst[i] = src[i];
    }
    dst[maxDstLen] = '\0';
}
