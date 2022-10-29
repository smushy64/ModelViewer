#pragma once
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

#define bool32 uint32_t

typedef u32 RendererID;
typedef i32 UniformID;
typedef usize VertexArrayID;
