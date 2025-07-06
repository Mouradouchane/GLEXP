#pragma once

#ifndef COMMON_TYPES_HPP
#define COMMON_TYPES_HPP

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
typedef int8_t	int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

// float's
typedef float  f32;
typedef double f64;
typedef long double f80;

#endif