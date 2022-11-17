/**
 * Description:  Utility functions
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 12, 2022 
 */
#pragma once
#include "pch.hpp"

/// @brief Get the length of a string
/// @param str string
/// @return length
usize strLen( const char* str );
/// @brief Get the length of a wstring
/// @param str string
/// @return length
usize strLenW( const wchar_t* str );
/// @brief Compare two strings to see if they are the same
/// @param strA string A
/// @param strB string B
/// @return true if they are the same, false if they aren't
bool  strCmp( const char* strA, const char* strB );
/// @brief Compare two wstrings to see if they are the same
/// @param strA string A
/// @param strB string B
/// @return true if they are the same, false if they aren't
bool  strCmpW( const wchar_t* strA, const wchar_t* strB );
/// @brief Convert char string to wchar_t string
/// @param src char string
/// @param dst wchar_t string buffer
/// @param dstMaxLen maximum allowed characters in destination buffer
void strTowstr( const char* src, wchar_t* dst, usize dstMaxLen );
/// @brief Convert wchar_t string to char string
/// @param src wchar_t string
/// @param dst char string buffer
/// @param dstMaxLen maximum allowed characters in destination buffer
void wstrTostr( const wchar_t* src, char* dst, usize dstMaxLen );

/// @brief string concatenation
/// @param srcASize size of src A
/// @param srcA source string A
/// @param srcBSize size of src B
/// @param srcB source string B
/// @param dstSize size of dst string
/// @param dst destination string
void strConcat(
    usize srcASize, const char* srcA,
    usize srcBSize, const char* srcB,
    usize dstSize, char* dst
);

/// @brief string concatenation
/// @param srcASize size of src A
/// @param srcA source string A
/// @param srcBSize size of src B
/// @param srcB source string B
/// @param dstSize size of dst string
/// @param dst destination string
void strConcatW(
    usize srcASize, const wchar_t* srcA,
    usize srcBSize, const wchar_t* srcB,
    usize dstSize, wchar_t* dst
);

/// @brief get position of substr in str
/// @param srcLen size of src str
/// @param src source str
/// @param subStrLen size of substr
/// @param subStr 
/// @param pos [out] position of substr
/// @return true if substr is found
bool getSubStrPos( usize strLen, const char* str, usize subStrLen, const char* subStr, usize* pos );

/// @brief clear substr from src str
/// @param srcLen size of src str
/// @param src source str
/// @param subStrLen size of substr
/// @param subStr sub str
/// @param maxDstLen maximum amount of characters in dst
/// @param dst [out] destination string buffer
void strClearSubStr(usize srcLen, const char* src,
    usize subStrLen, const char* subStr,
    usize maxDstLen, char* dst
);

/// @brief initialize a char buffer with spaces and terminate with null
void strInit( usize len, char* buffer );

bool isCharPresent( const char* str, char toFind );

bool findChar( const char* str, char toFind, usize* pos );

/// @brief copy null-terminated string to destination
void strCopy( const char* src, usize maxDstLen, char* dst );
