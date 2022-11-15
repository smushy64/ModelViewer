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
/// @brief Compare two strings to see if they are the same
/// @param strA string A
/// @param strB string B
/// @return true if they are the same, false if they aren't
bool  strCmp( const char* strA, const char* strB );
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
