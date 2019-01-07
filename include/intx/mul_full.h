// intx: extended precision integer library.
// Copyright 2018 Pawel Bylica.
// Licensed under the Apache License, Version 2.0. See the LICENSE file.

/// @file
/// This file provides implementations of full multiplication 64 x 64 -> 128.

#include <stdint.h>

struct mul_full_64_result
{
    uint64_t lo;
    uint64_t hi;
};

#if defined(__SIZEOF_INT128__)
#define INTX_HAVE_MULL_FULL_64_NATIVE 1

inline struct mul_full_64_result mul_full_64_native(uint64_t x, uint64_t y)
{
    unsigned __int128 p = (unsigned __int128)x * y;
    struct mul_full_64_result result = {(uint64_t)p, (uint64_t)(p >> 64)};
    return result;
}

#elif defined(_MSC_VER)
#define INTX_HAVE_MULL_FULL_64_NATIVE 1

#include <intrin.h>
inline struct mul_full_64_result mul_full_64_native(uint64_t x, uint64_t y)
{
    struct mul_full_64_result result;
    result.lo = _mul128(x, y, &result.hi);
    return result;
}

#else
#define INTX_HAVE_MULL_FULL_64_NATIVE 0
#endif

/// Generic and portable implementation of full 64 x 64 -> 128 multiplication
/// by fallback to four 32 x 32 -> 64 multiplications.
inline struct mul_full_64_result mul_full_64_generic(uint64_t x, uint64_t y)
{
    uint64_t xl = x & 0xffffffff;
    uint64_t xh = x >> 32;
    uint64_t yl = y & 0xffffffff;
    uint64_t yh = y >> 32;

    uint64_t t0 = xl * yl;
    uint64_t t1 = xh * yl;
    uint64_t t2 = xl * yh;
    uint64_t t3 = xh * yh;

    uint64_t u1 = t1 + (t0 >> 32);
    uint64_t u2 = t2 + (u1 & 0xffffffff);

    uint64_t lo = (u2 << 32) | (t0 & 0xffffffff);
    uint64_t hi = t3 + (u2 >> 32) + (u1 >> 32);
    struct mul_full_64_result result = {lo, hi};
    return result;
}

inline struct mul_full_64_result mul_full_64(uint64_t x, uint64_t y)
{
#if INTX_HAVE_MULL_FULL_64_NATIVE
    return mul_full_64_native(x, y);
#else
    return mul_full_64_generic(x, y);
#endif
}