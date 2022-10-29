#pragma once

#define ARRAY_COUNT(array) ( sizeof(array) / sizeof( (array)[0] ) )
#define TO_RAD(deg) ( deg * ( 3.141592f / 180.0f ) )
#define TO_DEG(rad) ( rad * ( 180.0f / 3.141592f ) )

#define KILOBYTES(num) ( num * 1024LL )
#define MEGABYTES(num) ( KILOBYTES(num) * 1024LL )
#define GIGABYTES(num) ( MEGABYTES(num) * 1024LL )
#define TERABYTES(num) ( GIGABYTES(num) * 1024LL )
