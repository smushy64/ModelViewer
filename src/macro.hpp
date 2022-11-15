/**
 * Description:  Simple macros
 * Author:       Alicia Amarilla (smushy)
 * File Created: November 05, 2022
*/

#ifndef SMUSHY_MACROS
#define SMUSHY_MACROS 1

/// @brief Check for nullptr before deleting heap-allocated pointer
/// @param ptr class pointer
#define SAFE_DELETE(ptr) if( ptr != nullptr ) \
    delete( ptr )\

/// @brief Calculate number of elements in a stack-allocated array
/// @param array array to calculate count of
#define ARRAY_COUNT(array) ( sizeof(array) / sizeof( array[0] ) )
/// @brief Convert Degrees to Radians
/// @param deg Degrees
#define TO_RAD(deg) ( deg * ( 3.141592f / 180.0f ) )
/// @brief Convert Radians to Degrees
/// @param rad Radians
#define TO_DEG(rad) ( rad * ( 180.0f / 3.141592f ) )

/// @brief Calculate byte-size of given kilobytes
/// @param count number of kilobytes
#define KILOBYTES(count) ( count * 1024LL )
/// @brief Calculate byte-size of given megabytes
/// @param count number of megabytes
#define MEGABYTES(count) ( KILOBYTES(count) * 1024LL )
/// @brief Calculate byte-size of given gigabytes
/// @param count number of gigabytes
#define GIGABYTES(count) ( MEGABYTES(count) * 1024LL )
/// @brief Calculate byte-size of given terabytes
/// @param count number of terabytes
#define TERABYTES(count) ( GIGABYTES(count) * 1024LL )

/// @brief Construct a 32-bit RGB value ( little-endian )
#define u32_RGB(r, g, b) r << 16 | g << 8 | b << 0
/// @brief Construct a 32-bit RGBA value ( little-endian )
#define u32_RGBA(r, g, b, a) a << 24 | r << 16 | g << 8 | b << 0

/// @brief loop between from and to
#define forrange( from, to ) for( isize i = from; i < to; ++i )

/// @brief loop between from and to in reverse
#define forrange_rev( from, to ) for( isize i = to; i > from; --i )

/// @brief loop for given cycles
#define forloop( cycles ) for( isize i = 0; i < cycles; ++i )

/// @brief swap pointer 1 and 2
#define swapPtr( ptr1, ptr2 ) auto* temp = ptr1; ptr1 = ptr2; ptr2 = temp

/// @brief macro for suppressing unused parameters
#define UNUSED_PARAM(param) (void)(param)

#endif
