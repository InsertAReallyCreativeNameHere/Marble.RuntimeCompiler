#pragma once

#if BUILD_TYPE_DYNAMIC
    #define coreapi __declspec(dllexport)
#else
    #define coreapi __declspec(dllimport)
#endif

#include <cstdint>

#define null NULL