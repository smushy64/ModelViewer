/**
 * Description:  Utility functions
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 12, 2022 
 */
#pragma once
#include "pch.hpp"

/// @brief Get the length of a null-terminated string.
/// IMPORTANT: Result DOES NOT include null-terminator!
usize stringLen( const char* str );
/// @brief Get the length of a null-terminated string.
/// IMPORTANT: Result DOES NOT include null-terminator!
usize stringLen( const wchar_t* str );

/// @brief Compare two strings to see if they are the same
/// @param strALen length of string a, including null-terminator
/// @param strA string A
/// @param strBLen length of string b, including null-terminator
/// @param strB string B
/// @return true if they are the same, false if they aren't
bool stringCmp( usize strALen, const char* strA, usize strBLen, const char* strB );
/// @brief Compare two strings to see if they are the same
/// @param strA string A
/// @param strB string B
/// @return true if they are the same, false if they aren't
bool stringCmp( const char* strA, const char* strB );
/// @brief Compare two strings to see if they are the same
/// @param strALen length of string a, including null-terminator
/// @param strA string A
/// @param strBLen length of string b, including null-terminator
/// @param strB string B
/// @return true if they are the same, false if they aren't
bool stringCmp( usize strALen, const wchar_t* strA, usize strBLen, const wchar_t* strB );
/// @brief Compare two wstrings to see if they are the same
/// @param strA string A
/// @param strB string B
/// @return true if they are the same, false if they aren't
bool stringCmp( const wchar_t* strA, const wchar_t* strB );

/// @brief Convert char string to wchar_t string
/// @param src char string
/// @param dst wchar_t string buffer
/// @param dstSize maximum allowed characters in destination buffer
void stringToWstring( const char* src, usize dstSize, wchar_t* dst );
/// @brief Convert wchar_t string to char string
/// @param src wchar_t string
/// @param dstSize maximum allowed characters in destination buffer
/// @param dst char string buffer
void wstringToString( const wchar_t* src, usize dstSize, char* dst );

/// @brief string concatenation. result is null-terminated.
/// @param srcALen length of source string A, including null-terminator
/// @param srcA source string A
/// @param srcBLen length of source string B, including null-terminator
/// @param srcB source string B
/// @param dstSize length of destination string, including null-terminator
/// @param dst destination string
void stringConcat(
    usize srcALen, const char* srcA,
    usize srcBLen, const char* srcB,
    usize dstSize, char* dst
);

/// @brief string concatenation. result is null-terminated.
/// @param srcA source string A
/// @param srcB source string B
/// @param dstSize size of dst string including null-terminator
/// @param dst destination string
void stringConcat(
    const char* srcA,
    const char* srcB,
    usize dstSize, char* dst
);

/// @brief string concatenation. result is null-terminated.
/// @param srcALen length of source string A, including null-terminator
/// @param srcA source string A
/// @param srcBLen length of source string B, including null-terminator
/// @param srcB source string B
/// @param dstSize length of destination string, including null-terminator
/// @param dst destination string
void stringConcat(
    usize srcALen, const wchar_t* srcA,
    usize srcBLen, const wchar_t* srcB,
    usize dstSize, wchar_t* dst
);

/// @brief string concatenation. result is null-terminated.
/// @param srcA source string A
/// @param srcB source string B
/// @param dstSize size of dst string including null-terminator
/// @param dst destination string
void stringConcat(
    const wchar_t* srcA,
    const wchar_t* srcB,
    usize dstSize, wchar_t* dst
);

/// @brief get position of sub-string in source string
/// @param srcLen length of source string, including null-terminator
/// @param src source string
/// @param subStrLen length of sub-string, including null-terminator
/// @param subStr sub-string
/// @param result [out] pointer to usize to hold result
/// @return true if position was found, false if it wasn't
bool subStringPos(
    usize srcLen,
    const char* src,
    usize subStrLen,
    const char* subStr,
    usize* result
);
/// @brief get position of sub-string in source string
/// @param src source string
/// @param subStr sub-string
/// @param result [out] pointer to usize to hold result
/// @return true if position was found, false if it wasn't
bool subStringPos(
    const char* src,
    const char* subStr,
    usize* result
);
/// @brief get position of sub-string in source string
/// @param srcLen length of source string, including null-terminator
/// @param src source string
/// @param subStrLen length of sub-string, including null-terminator
/// @param subStr sub-string
/// @param result [out] pointer to usize to hold result
/// @return true if position was found, false if it wasn't
bool subStringPos(
    usize srcLen,
    const wchar_t* src,
    usize subStrLen,
    const wchar_t* subStr,
    usize* result
);
/// @brief get position of sub-string in source string
/// @param src source string
/// @param subStr sub-string
/// @param result [out] pointer to usize to hold result
/// @return true if position was found, false if it wasn't
bool subStringPos(
    const wchar_t* src,
    const wchar_t* subStr,
    usize* result
);

/// @brief delete sub-string from source string and store in given destination buffer
/// @param srcLen length of source string, including null-terminator
/// @param src source string
/// @param subStrLen length of sub-string, including null-terminator
/// @param subStr sub-string
/// @param dstSize length of destination buffer, including null-terminator
/// @param dst destination buffer
void deleteSubString(
    usize srcLen,
    const char* src,
    usize subStrLen,
    const char* subStr,
    usize dstSize,
    char* dst
);
/// @brief delete sub-string from source string and store in given destination buffer
/// @param src source string
/// @param subStr sub-string
/// @param dstSize length of destination buffer, including null-terminator
/// @param dst destination buffer
void deleteSubString(
    const char* src,
    const char* subStr,
    usize dstSize,
    char* dst
);
/// @brief delete sub-string from source string and store in given destination buffer
/// @param srcLen length of source string, including null-terminator
/// @param src source string
/// @param subStrLen length of sub-string, including null-terminator
/// @param subStr sub-string
/// @param dstSize length of destination buffer, including null-terminator
/// @param dst destination buffer
void deleteSubString(
    usize srcLen,
    const wchar_t* src,
    usize subStrLen,
    const wchar_t* subStr,
    usize dstSize,
    wchar_t* dst
);
/// @brief delete sub-string from source string and store in given destination buffer
/// @param src source string
/// @param subStr sub-string
/// @param dstSize length of destination buffer, including null-terminator
/// @param dst destination buffer
void deleteSubString(
    const wchar_t* src,
    const wchar_t* subStr,
    usize dstSize,
    wchar_t* dst
);

/// @brief initialize a string buffer. clear with spaces and add null-terminator
/// @param size size of buffer
/// @param buffer pointer to buffer
void initStringBuffer( usize size, char* buffer );
/// @brief initialize a string buffer. clear with spaces and add null-terminator
/// @param size size of buffer
/// @param buffer pointer to buffer
void initStringBuffer( usize size, wchar_t* buffer );

/// @brief Check if string contains character
/// @param strLen length of string, including null-terminator
/// @param str string to check
/// @param charToFind character to look for
/// @return true if character found
bool stringContains( usize strLen, const char* str, char charToFind );
/// @brief Check if string contains character
/// @param str string to check
/// @param charToFind character to look for
/// @return true if character found
bool stringContains( const char* str, char charToFind );
/// @brief Check if string contains character
/// @param strLen length of string, including null-terminator
/// @param str string to check
/// @param charToFind character to look for
/// @return true if character found
bool stringContains( usize strLen, const wchar_t* str, wchar_t charToFind );
/// @brief Check if string contains character
/// @param str string to check
/// @param charToFind character to look for
/// @return true if character found
bool stringContains( const wchar_t* str, wchar_t charToFind );

/// @brief Get the first position of a character in string
/// @param strLen length of string, including null-terminator
/// @param str string to check
/// @param charToFind character to look for
/// @param result [out] position of character
/// @return true if character found
bool getCharPosition( usize strLen, const char* str, char charToFind, usize* result );
/// @brief Get the first position of a character in string
/// @param str string to check
/// @param charToFind character to look for
/// @param result [out] position of character
/// @return true if character found
bool getCharPosition( const char* str, char charToFind, usize* result );
/// @brief Get the first position of a character in string
/// @param strLen length of string, including null-terminator
/// @param str string to check
/// @param charToFind character to look for
/// @param result [out] position of character
/// @return true if character found
bool getCharPosition( usize strLen, const wchar_t* str, wchar_t charToFind, usize* result );
/// @brief Get the first position of a character in string
/// @param str string to check
/// @param charToFind character to look for
/// @param result [out] position of character
/// @return true if character found
bool getCharPosition( const wchar_t* str, wchar_t charToFind, usize* result );

/// @brief Copy source string to destination buffer.
/// If length of source string is less than the destination buffer's size, it will only copy up to that length.
/// Otherwise, if destination buffer size is less than source string length, it will only copy up to buffer size.
/// @param srcLen length of source string, including null-terminator
/// @param src source string
/// @param dstSize size of destination buffer, including null-terminator
/// @param dst destination buffer
void stringCopy( usize srcLen, const char* src, usize dstSize, char* dst );
/// @brief Copy source string to destination buffer.
/// If length of source string is less than the destination buffer's size, it will only copy up to that length.
/// Otherwise, if destination buffer size is less than source string length, it will only copy up to buffer size.
/// @param src source string
/// @param dstSize size of destination buffer, including null-terminator
/// @param dst destination buffer
void stringCopy( const char* src, usize dstSize, char* dst );
/// @brief Copy source string to destination buffer.
/// If length of source string is less than the destination buffer's size, it will only copy up to that length.
/// Otherwise, if destination buffer size is less than source string length, it will only copy up to buffer size.
/// @param srcLen length of source string, including null-terminator
/// @param src source string
/// @param dstSize size of destination buffer, including null-terminator
/// @param dst destination buffer
void stringCopy( usize srcLen, const wchar_t* src, usize dstSize, wchar_t* dst );
/// @brief Copy source string to destination buffer.
/// If length of source string is less than the destination buffer's size, it will only copy up to that length.
/// Otherwise, if destination buffer size is less than source string length, it will only copy up to buffer size.
/// @param src source string
/// @param dstSize size of destination buffer, including null-terminator
/// @param dst destination buffer
void stringCopy( const wchar_t* src, usize dstSize, wchar_t* dst );

/// My own implementation of std::vector
class DynList {
    /// @brief reserve space
    /// @param capacity how many elements should be allocated
    /// @param elementSize size of each element in bytes
    DynList( usize capacity, usize elementSize );
    /// @brief allocate extra space by element count. Bytes allocated will be (elementCount * elementSize) + previous size
    /// @param elementCount number of new elements to alloc
    void alloc( usize elementCount );
    /// @brief push new element into list. 
    /// @param dataToCopy 
    void push( void* dataToCopy );
private:
    usize m_elementSize; // size of each element
    usize m_elementCapacity; // capacity of list ( how many elements it can hold )
    usize m_elementCount; // how many elements exist in list
    usize m_capacity; // capacity of list ( how much memory it has allocated )
    usize m_size; // occupied bytes ( out of capacity )
    void* m_data;

};
