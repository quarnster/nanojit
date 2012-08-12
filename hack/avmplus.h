#ifndef __INCLUDED_AVMPLUS_H
#define __INCLUDED_AVMPLUS_H

#include <stddef.h>
#include <stdint.h>
#include <memory.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include <sys/mman.h>

namespace avmplus
{
    static inline void AvmLog(const char * fmt, ...)
    {
        va_list va;
        va_start(va, fmt);
        vprintf(fmt, va);
        va_end(va);
    }

    static inline void AvmAssertFail(const char *msg)
    {
        assert(msg);
    }
    class AvmLogControl
    {
        void printf( const char* format, ... );
    };
    enum
    {
        ACCSET_NONE,
        ACCSET_VARS,
        ACCSET_TAGS,
        ACCSET_OTHER
    };
}

#include <iostream>
class Core
{
public:
    Core()
    : console(std::cout)
    {

    }
    std::ostream &console;

    bool operator!=(const void *) const
    {
        return false;
    }

    const Core* operator->() const
    {
        return this;
    }

};

static Core core;
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
#define VMPI_strncat strncat
#define VMPI_strcat strcat
#define VMPI_strlen strlen
#define VMPI_vsnprintf vsnprintf
#define VMPI_snprintf snprintf
#define VMPI_sprintf sprintf
#define VMPI_isdigit isdigit

#define PERFM_NVPROF(a, b)

#define AvmAssert assert

#define mmfx_alloc(bytes) malloc(bytes)
#define mmfx_free(pointer) ::free(pointer)

#define FASTCALL

#define AVMPI_allocateCodeMemory(bytes)  mmap(NULL, bytes, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANON, -1, 0)
#define AVMPI_freeCodeMemory(addr, bytes) munmap(addr, bytes)
#define AVMPI_makeCodeMemoryExecutable(addr, nbytes, read)


static inline int VMPI_getVMPageSize()
{
    return 4096;
}

#endif

