#ifndef __INCLUDED_AVMPLUS_H
#define __INCLUDED_AVMPLUS_H

#include <stddef.h>
#include <stdint.h>
#include <memory.h>

namespace avmplus
{

}

typedef struct
{
    union
    {
        float f[4];
        struct { float x; float y; float z; float w; };
    };
} float4_t;


#define f4_x(f) (f).x
#define f4_y(f) (f).y
#define f4_z(f) (f).z
#define f4_w(f) (f).w
#define VMPI_memset memset


#endif

