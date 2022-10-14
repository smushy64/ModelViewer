#pragma once

#define ARRAY_COUNT(array) ( sizeof(array) / sizeof( (array)[0] ) )

#define KILOBYTES(num) ( num * 1024LL )
#define MEGABYTES(num) ( KILOBYTES(num) * 1024LL )
#define GIGABYTES(num) ( MEGABYTES(num) * 1024LL )
#define TERABYTES(num) ( GIGABYTES(num) * 1024LL )
