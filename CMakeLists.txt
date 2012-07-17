cmake_minimum_required(VERSION 2.8)

include_directories(nanojit hack hack/vprof)

set(CMAKE_CXX_FLAGS "-DFEATURE_NANOJIT -DAVMPLUS_AMD64 -DNANOJIT_64BIT -DVMCFG_NANOJIT")

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -D_DEBUG -DNJ_VERBOSE")
endif()

message(${CMAKE_CXX_FLAGS})

add_executable(test
    main.cpp
    core/LirHelper.cpp
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
