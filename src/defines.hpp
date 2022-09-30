#pragma once
#include "debug.hpp"
#include <cstdint>

#define usize uintptr_t
#define isize intptr_t

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#define i8  int8_t
#define i16 int16_t
#define i32 int32_t
#define i64 int64_t

#define f32 float
#define f64 double

#define loop while( true )

#define ARRAY_COUNT(array) ( sizeof(array) / sizeof( (array)[0] ) )

#define KILOBYTES(expr) ( expr * 1024LL )
#define MEGABYTES(expr) ( KILOBYTES(expr) * 1024LL )
#define GIGABYTES(expr) ( MEGABYTES(expr) * 1024LL )
#define TERABYTES(expr) ( GIGABYTES(expr) * 1024LL )

typedef i32 FontID;
typedef i32 UniformID;
typedef u32 RendererID;
