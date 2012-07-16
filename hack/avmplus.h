#ifndef __INCLUDED_AVMPLUS_H
#define __INCLUDED_AVMPLUS_H

#include <stddef.h>
#include <stdint.h>
#include <memory.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

namespace avmplus
{
    static inline void AvmLog(const char * fmt, ...)
    {
        va_list va;
        va_start(va, fmt);
        vprintf(fmt, va);
        va_end(va);
    }

}

struct float4_t
{

    float x;
    float y;
    float z;
    float w;
};

static inline float4_t operator+(const float4_t& self, const float4_t& other)
{
    float4_t ret = {self.x+other.x, self.y+other.y, self.z+other.z, self.w+other.w};
    return ret;
}
static inline float4_t operator-(const float4_t& self, const float4_t& other)
{
    float4_t ret = {self.x-other.x, self.y-other.y, self.z-other.z, self.w-other.w};
    return ret;
}
static inline float4_t operator*(const float4_t& self, const float4_t& other)
{
    float4_t ret = {self.x*other.x, self.y*other.y, self.z*other.z, self.w*other.w};
    return ret;
}
static inline float4_t operator/(const float4_t& self, const float4_t& other)
{
    float4_t ret = {self.x/other.x, self.y/other.y, self.z/other.z, self.w/other.w};
    return ret;
}

static inline bool operator==(const float4_t& self, const float4_t& other)
{
    return self.x == other.x && self.y == other.y && self.z == other.z && self.w == other.w;
}

#define f4_x(f) (f).x
#define f4_y(f) (f).y
#define f4_z(f) (f).z
#define f4_w(f) (f).w
#define f4_add(a, b) (a) + (b)
#define f4_sub(a, b) (a) - (b)
#define f4_mul(a, b) (a) * (b)
#define f4_div(a, b) (a) / (b)
#define f4_eq_i(a, b) (a) == (b)

#define VMPI_memset memset
#define VMPI_abort abort
#define VMPI_memcmp memcmp
#define VMPI_strcpy strcpy
#define VMPI_strlen strlen

#define PERFM_NVPROF(a, b)

static inline int VMPI_getVMPageSize()
{
    return 0;
}

#endif

