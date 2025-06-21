#ifndef DEFS
#define DEFS

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

/* float type definitions for ease of reasoning about size */
typedef float f32;  /* 4-byte floating point number (float) */
typedef double f64; /* 8-byte floating point number (double) */

typedef unsigned int uint;
typedef volatile uint32_t reg32;

#endif /* DEFS */
