cmake_minimum_required(VERSION 2.8)

project(nanojit)
include_directories(nanojit . lirasm/VMPI.nj hack hack/vprof)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFEATURE_NANOJIT -DVMCFG_NANOJIT -DVMCFG_FLOAT")

if(NOT ANDROID)
    if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAVMPLUS_AMD64 -DNANOJIT_64BIT -DVMCFG_SSE2")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAVMPLUS_IA32 -DVMCFG_SSE2")
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAVMPLUS_UNIX")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAVMPLUS_ARM -DAVMPLUS_UNIX")
endif()

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -D_DEBUG -DNJ_VERBOSE")
endif()


set(NANOJIT_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "Nanojit cxx flags")

add_library(nanojit
    nanojit/Allocator.cpp
    nanojit/Assembler.cpp
    nanojit/CodeAlloc.cpp
    nanojit/Containers.cpp
    nanojit/Fragmento.cpp
    nanojit/LIR.cpp
    nanojit/NativeARM.cpp
    nanojit/NativeMIPS.cpp
    nanojit/NativePPC.cpp
    nanojit/NativeSH4.cpp
#    nanojit/NativeSparc.cpp
    nanojit/NativeThumb2.cpp
    nanojit/NativeX64.cpp
    nanojit/Nativei386.cpp
    nanojit/RegAlloc.cpp
    nanojit/njconfig.cpp
)


add_executable(lirasm
    lirasm/lirasm/lirasm.cpp
    lirasm/VMPI.nj/VMPI.cpp
    lirasm/VMPI.nj/avmplus.cpp
)
target_link_libraries(lirasm nanojit)
#add_executable(test
#    main.cpp
#)

#target_link_libraries(test nanojit)
