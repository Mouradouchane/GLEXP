#pragma once

#ifndef CORE_TYPES_HPP
#define CORE_TYPES_HPP

#include <stdint.h>

// pointers for sizes
typedef void*     ptr;
typedef uint8_t*  ptr8;
typedef uint16_t* ptr16;
typedef uint32_t* ptr32;
typedef uint64_t* ptr64;

// unsigned int's
typedef uint8_t  u8 , byte;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// int's
typedef int8_t	int8  , i8;
typedef int16_t int16 , i16;
typedef int32_t int32 , i32;
typedef int64_t int64 , i64;

// float's
typedef float  f32;
typedef double f64;
typedef long double f80;

// vector's for none math use
template<typename type> struct t_vec2 {
	type x, y;
};

template<typename type> struct t_vec3 {
	type x, y, z;
};

template<typename type> struct t_vec4 {
	type x, y, z, w;
};
// ==========================

// events 
struct event_handle {
	u32 index1;
	u32 index2;
};

// keys
typedef u32 h_key32;
typedef u64 h_key64;

#endif