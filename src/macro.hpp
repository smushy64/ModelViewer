#pragma once

// Check for nullptr before calling ->Release()
#define COM_RELEASE(expr) if( expr != nullptr ) \
    expr->Release()\

// Check for nullptr before using delete
#define SAFE_DEL(expr) if( expr != nullptr ) \
    delete expr\

#define ARRAY_COUNT(array) ( sizeof(array) / sizeof( (array)[0] ) )
#define TO_RAD(deg) ( deg * ( 3.141592f / 180.0f ) )
#define TO_DEG(rad) ( rad * ( 180.0f / 3.141592f ) )

#define KILOBYTES(num) ( num * 1024LL )
#define MEGABYTES(num) ( KILOBYTES(num) * 1024LL )
#define GIGABYTES(num) ( MEGABYTES(num) * 1024LL )
#define TERABYTES(num) ( GIGABYTES(num) * 1024LL )
