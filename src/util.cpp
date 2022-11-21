/**
 * Description:  Utility functions
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 12, 2022 
 */
#include "util.hpp"
#include "pch.hpp"
#include "platform/io.hpp"

void stringToWstring( const char* src, usize dstSize, wchar_t* dst ) {
    mbstowcs( &dst[0], src, dstSize );
}
void wstringToString( const wchar_t* src, usize dstSize, char* dst ) {
    wcstombs( &dst[0], src, dstSize );
}

usize stringLen( const char* str ) {
    usize result = 0;
    while(*str++) {
        ++result;
    }
    return result;
}
usize stringLen( const wchar_t* str ) {
    usize result = 0;
    while(*str++) {
        ++result;
    }
    return result;
}

bool stringCmp( usize strALen, const char* strA, usize strBLen, const char* strB ) {
    if( strALen != strBLen ) {
        return false;
    }

    ucycles( strALen ) {
        if( strA[i] != strB[i] ) {
            return false;
        }
    }

    return true;
}
bool stringCmp( const char* strA, const char* strB ) {
    usize strALen = stringLen(strA) + 1;
    usize strBLen = stringLen(strB) + 1;
    if( strALen != strBLen ) {
        return false;
    }

    ucycles( strALen ) {
        if( strA[i] != strB[i] ) {
            return false;
        }
    }

    return true;
}
bool stringCmp( usize strALen, const wchar_t* strA, usize strBLen, const wchar_t* strB ) {
    if( strALen != strBLen ) {
        return false;
    }

    ucycles( strALen ) {
        if( strA[i] != strB[i] ) {
            return false;
        }
    }

    return true;
}
bool stringCmp( const wchar_t* strA, const wchar_t* strB ) {
    usize strALen = stringLen(strA) + 1;
    usize strBLen = stringLen(strB) + 1;
    if( strALen != strBLen ) {
        return false;
    }

    ucycles( strALen ) {
        if( strA[i] != strB[i] ) {
            return false;
        }
    }

    return true;
}

void stringConcat(
    usize srcALen, const char* srcA,
    usize srcBLen, const char* srcB,
    usize dstLen, char* dst
) {
    usize nullPosition = dstLen - 1;
    usize targetLen = srcALen + srcBLen;

    for( usize i = 0; i < srcALen; ++i ) {
        if( i >= dstLen ) {
            dst[nullPosition] = '\0';
            return;
        }
        dst[i] = srcA[i];
    }
    for( usize i = srcALen; i < targetLen; ++i ) {
        if( i >= dstLen ) {
            dst[nullPosition] = '\0';
            return;
        }
        dst[i] = srcB[i - srcALen];
    }
    dst[nullPosition] = '\0';
}
void stringConcat(
    const char* srcA,
    const char* srcB,
    usize dstLen, char* dst
) {
    stringConcat( stringLen( srcA ) + 1, srcA, stringLen( srcB ) + 1, srcB, dstLen, dst );
}
void stringConcat(
    usize srcALen, const wchar_t* srcA,
    usize srcBLen, const wchar_t* srcB,
    usize dstLen, wchar_t* dst
) {
    usize nullPosition = dstLen - 1;
    usize targetLen = srcALen + srcBLen;

    for( usize i = 0; i < srcALen; ++i ) {
        if( i >= dstLen ) {
            dst[nullPosition] = '\0';
            return;
        }
        dst[i] = srcA[i];
    }
    for( usize i = srcALen; i < targetLen; ++i ) {
        if( i >= dstLen ) {
            dst[nullPosition] = '\0';
            return;
        }
        dst[i] = srcB[i - srcALen];
    }
    dst[nullPosition] = '\0';
}
void stringConcat(
    const wchar_t* srcA,
    const wchar_t* srcB,
    usize dstLen, wchar_t* dst
) {
    stringConcat( stringLen( srcA ) + 1, srcA, stringLen( srcB ) + 1, srcB, dstLen, dst );
}

bool subStringPos(
    usize srcLen,
    const char* src,
    usize subStrLen,
    const char* subStr,
    usize* result
) {
    ucycles( srcLen ) {
        if( src[i] == subStr[0] ) {
            if( srcLen - i < subStrLen ) {
                return false;
            }

            if( stringCmp( subStrLen, &src[i], subStrLen, subStr ) ) {
                *result = i;
                return true;
            }
        }
    }
    return false;
}
bool subStringPos(
    const char* src,
    const char* subStr,
    usize* result
) {
    return subStringPos( stringLen( src ) + 1, src, stringLen( subStr ) + 1, subStr, result );
}
bool subStringPos(
    usize srcLen,
    const wchar_t* src,
    usize subStrLen,
    const wchar_t* subStr,
    usize* result
) {
    ucycles( srcLen ) {
        if( src[i] == subStr[0] ) {
            if( srcLen - i < subStrLen ) {
                return false;
            }

            if( stringCmp( subStrLen, &src[i], subStrLen, subStr ) ) {
                *result = i;
                return true;
            }
        }
    }
    return false;
}
bool subStringPos(
    const wchar_t* src,
    const wchar_t* subStr,
    usize* result
) {
    return subStringPos( stringLen( src ) + 1, src, stringLen( subStr ) + 1, subStr, result );
}

void deleteSubString(
    usize srcLen,
    const char* src,
    usize subStrLen,
    const char* subStr,
    usize dstLen,
    char* dst
) {
    usize subStringPosition = 0;
    if( !subStringPos( srcLen, src, subStrLen, subStr, &subStringPosition ) ) {
        return;
    }

    usize nullPosition = dstLen - 1;

    ucycles( srcLen ) {
        if( i >= nullPosition ) {
            dst[nullPosition] = '\0';
            return;
        }

        if( i < subStringPosition || i >= subStringPosition + subStrLen ) {
            dst[i] = src[i];
        }
    }

    dst[nullPosition] = '\0';
}
void deleteSubString(
    const char* src,
    const char* subStr,
    usize dstLen,
    char* dst
) {
    deleteSubString(
        stringLen( src ) + 1,
        src,
        stringLen( subStr ) + 1,
        subStr,
        dstLen,
        dst
    );
}
void deleteSubString(
    usize srcLen,
    const wchar_t* src,
    usize subStrLen,
    const wchar_t* subStr,
    usize dstLen,
    wchar_t* dst
) {
    usize subStringPosition = 0;
    if( !subStringPos( srcLen, src, subStrLen, subStr, &subStringPosition ) ) {
        return;
    }

    usize nullPosition = dstLen - 1;

    ucycles( srcLen ) {
        if( i >= nullPosition ) {
            dst[nullPosition] = '\0';
            return;
        }

        if( i < subStringPosition || i >= subStringPosition + subStrLen ) {
            dst[i] = src[i];
        }
    }

    dst[nullPosition] = '\0';
}
void deleteSubString(
    const wchar_t* src,
    const wchar_t* subStr,
    usize dstLen,
    wchar_t* dst
) {
    deleteSubString(
        stringLen( src ) + 1,
        src,
        stringLen( subStr ) + 1,
        subStr,
        dstLen,
        dst
    );
}

void initStringBuffer( usize size, char* buffer ) {
    ucycles( size ) {
        buffer[i] = ' ';
    }
    usize nullPostion = size - 1;

    buffer[nullPostion] = '\0';
}
void initStringBuffer( usize size, wchar_t* buffer ) {
    ucycles( size ) {
        buffer[i] = ' ';
    }
    usize nullPostion = size - 1;

    buffer[nullPostion] = '\0';
}

bool stringContains( usize strLen, const char* str, char charToFind ) {
    ucycles( strLen ) {
        if( str[strLen] == charToFind ) {
            return true;
        }
    }
    return false;
}
bool stringContains( const char* str, char charToFind ) {
    return stringContains( stringLen( str ) + 1, str, charToFind );
}
bool stringContains( usize strLen, const wchar_t* str, wchar_t charToFind ) {
    ucycles( strLen ) {
        if( str[strLen] == charToFind ) {
            return true;
        }
    }
    return false;
}
bool stringContains( const wchar_t* str, wchar_t charToFind ) {
    return stringContains( stringLen( str ) + 1, str, charToFind );
}

bool getCharPosition( usize strLen, const char* str, char charToFind, usize* result ) {
    ucycles( strLen ) {
        if( str[i] == charToFind ) {
            *result = i;
            return true;
        }
    }
    return false;
}
bool getCharPosition( const char* str, char charToFind, usize* result ) {
    return getCharPosition( stringLen( str ) + 1, str, charToFind, result );
}
bool getCharPosition( usize strLen, const wchar_t* str, wchar_t charToFind, usize* result ) {
    ucycles( strLen ) {
        if( str[i] == charToFind ) {
            *result = i;
            return true;
        }
    }
    return false;
}
bool getCharPosition( const wchar_t* str, wchar_t charToFind, usize* result ) {
    return getCharPosition( stringLen( str ) + 1, str, charToFind, result );
}

void stringCopy( usize srcLen, const char* src, usize dstSize, char* dst ) {
    usize charsToCopy = srcLen < dstSize ? srcLen : dstSize;
    ucycles( charsToCopy ) {
        dst[i] = src[i];
    }
    usize nullPosition = dstSize - 1;

    dst[nullPosition] = '\0';
}
void stringCopy( const char* src, usize dstSize, char* dst ) {
    stringCopy( stringLen(src) + 1, src, dstSize, dst );
}
void stringCopy( usize srcLen, const wchar_t* src, usize dstSize, wchar_t* dst ) {
    usize charsToCopy = srcLen < dstSize ? srcLen : dstSize;
    ucycles( charsToCopy ) {
        dst[i] = src[i];
    }
    usize nullPosition = dstSize - 1;

    dst[nullPosition] = '\0';
}
void stringCopy( const wchar_t* src, usize dstSize, wchar_t* dst ) {
    stringCopy( stringLen(src) + 1, src, dstSize, dst );
}
