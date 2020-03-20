/*
    Copyright (c) 2002-2010 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file opencl_types.hh
 *
 * Types visible both to the kernel and host code.
 *
 * @author 2009 Carlos Sanchéz de La Lama (csanchez @ urjc.es)
 * @author 2009-2010 Pekka Jääskeläinen (pekka jaaskelainen tut fi)
 */


#ifndef _OPENCL_TCE_TYPES_HH
#define _OPENCL_TCE_TYPES_HH

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#define __global

typedef unsigned int uint;
typedef unsigned char uchar;

/****************************************************************************
 * Datatypes
 ****************************************************************************/

/*
 * float types
 */

struct float2 {
    float x;
    float y;
    float2() : x(0.0f), y(0.0f) {}
    float2(float x_) : x(x_), y(0.0f) {}
    float2(int x_) : x((float)x_), y(0.0f) {}
    float2(float x_, float y_) : x(x_), y(y_) {}

    inline
    float& operator[](int i) {
        if (i == 0) return x;
        if (i == 1) return y;
        return y;
    }
};

struct float4 {
    float x;
    float y;
    float z;
    float w;
    float4() : x(0.0f), y(0.0f) {}
    float4(float x_) : x(x_), y(0.0f) {}
    float4(int x_) : x((float)x_), y(0.0f) {}
    float4(float x_, float y_) : x(x_), y(y_) {}

    inline
    float4& operator+=(const float4& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }

    inline
    float4& operator-=(const float4& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }

    inline
    float4& operator*=(const float& other) {
        x *= other;
        y *= other;
        z *= other;
        w *= other;
        return *this;
    }

};


/*
 * uchar types
 */

struct uchar4 {
    uchar x;
    uchar y;
    uchar z;
    uchar w;
};


/**
 * int types
 */

struct int2 {
    int32_t x;
    int32_t y;
    int2() : x(0), y(0) {}
    int2(int32_t x_) : x(x_), y(0) {}
    int2(int32_t x_, int32_t y_) : x(x_), y(y_) {}

    inline
    int32_t& operator[](int i) {
        if (i == 0) return x;
        if (i == 1) return y;
        return y;
    }

    inline
    int2& operator+=(const int2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

};


/****************************************************************************
 * Operators
 ****************************************************************************/


inline
float2 operator+(const float2 a, const float2 b) {
    float2 newFloat2;
    newFloat2.x = a.x + b.x;
    newFloat2.y = a.y + b.y;
    return newFloat2;
}

inline
float2 operator-(const float2 a, const float2 b) {
    float2 newFloat2;
    newFloat2.x = a.x - b.x;
    newFloat2.y = a.y - b.y;
    return newFloat2;
}

inline
float2 operator*(const float2 a, const int scalar) {
    float2 newFloat2;
    newFloat2.x = a.x*scalar;
    newFloat2.y = a.y*scalar;
    return newFloat2;
}

inline
float2 operator*(const int scalar, const float2 a) {
    return a*scalar;
}

inline
float4 operator*(const float4 a, const int scalar) {
    float4 newFloat;
    newFloat.x = a.x*scalar;
    newFloat.y = a.y*scalar;
    newFloat.z = a.z*scalar;
    newFloat.w = a.w*scalar;
    return newFloat;
}

inline
float4 operator*(const float4 a, const float scalar) {
    float4 newFloat;
    newFloat.x = a.x*scalar;
    newFloat.y = a.y*scalar;
    newFloat.z = a.z*scalar;
    newFloat.w = a.w*scalar;
    return newFloat;
}

inline
float4 operator*(const int scalar, const float4 a) {
    return a*scalar;
}

inline
float4 operator*(const float scalar, const float4 a) {
    return a*scalar;
}

inline
float4 operator+(const float4 a, const float4 b) {
    float4 newFloat;
    newFloat.x = a.x + b.x;
    newFloat.y = a.y + b.y;
    newFloat.z = a.z + b.z;
    newFloat.w = a.w + b.w;
    return newFloat;
}


inline
float4 operator-(const float4 a, const float4 b) {
    float4 newFloat;
    newFloat.x = a.x - b.x;
    newFloat.y = a.y - b.y;
    newFloat.z = a.z - b.z;
    newFloat.w = a.w - b.w;
    return newFloat;
}

inline
float4 operator*(const float4 a, const float4 b) {
    float4 newFloat;
    newFloat.x = a.x * b.x;
    newFloat.y = a.y * b.y;
    newFloat.z = a.z * b.z;
    newFloat.w = a.w * b.w;
    return newFloat;
}

inline
int2 operator-(const int2 a, const int2 b) {
    int2 newVal;
    newVal.x = a.x - b.x;
    newVal.y = a.y - b.y;
    return newVal;
}


/****************************************************************************
 * Explicit Conversions
 ****************************************************************************/

inline
float4 convert_float4(uchar4 src) {
    float4 target;
    target.x = (float)src.x;
    target.y = (float)src.y;
    target.z = (float)src.z;
    target.w = (float)src.w;
    return target;
}

/****************************************************************************
 * Integer Functions
 ****************************************************************************/

template <typename gentype>
gentype mul24(gentype x, gentype y) {
    return (x & 0x00FFFFFF) * (y & 0x00FFFFFF);
}

template <typename gentype>
gentype mad24(gentype x, gentype y, gentype z) {
    return mul24(x, y) + z;    
}

template <typename gentype>
gentype mad(gentype x, gentype y, gentype z) {
    return x * y + z;    
}

/****************************************************************************
 * Math Functions
 ****************************************************************************/

template <typename gentype>
gentype native_cos(gentype x) {
    return cos(x);
}

template <typename gentype>
gentype native_sin(gentype x) {
    return sin(x);
}

/****************************************************************************
 * Geometric Functions
 ****************************************************************************/

inline float 
dot(const float4& v, const float4& w) {
  return v.x * w.x + v.y * w.y + v.z * w.z + v.w * w.w;
}

inline float
fract(const float x, __global float* iptr) {
    float flr = floor(x);
    *iptr = flr;
    return fmin(x - flr, 0.9999999999999999999e-1f);
}

/****************************************************************************
 * Common Functions
 ****************************************************************************/

inline float4
mix(float4 x, float4 y, float a) {
    return x + (y - x) * a;
}

/****************************************************************************
 * Implementation defined
 ****************************************************************************/

struct _thread_context {
    uint work_dim;
    size_t global_size[3];
    size_t global_id[3];
    size_t local_size[3];
    size_t local_id[3];
    size_t num_groups[3];
    size_t group_id[3];
};


#endif
