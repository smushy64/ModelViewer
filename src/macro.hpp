#pragma once

// Check for nullptr before using delete
#define SAFE_DEL(ptr) if( ptr != nullptr ) \
    delete( ptr )\

#define ARRAY_COUNT(array) ( sizeof(array) / sizeof( array[0] ) )
#define TO_RAD(deg) ( deg * ( 3.141592f / 180.0f ) )
#define TO_DEG(rad) ( rad * ( 180.0f / 3.141592f ) )

#define BYTES(count)     (           count  * 1LL    )
#define KILOBYTES(count) (     BYTES(count) * 1024LL )
#define MEGABYTES(count) ( KILOBYTES(count) * 1024LL )
#define GIGABYTES(count) ( MEGABYTES(count) * 1024LL )
#define TERABYTES(count) ( GIGABYTES(count) * 1024LL )
