#ifndef __INC_H__
#define __INC_H__

#pragma once

#if BUILD_TYPE_DYNAMIC
    #define coreapi __declspec(dllexport)
#else
    #define coreapi __declspec(dllimport)
#endif

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <chrono>

#include <future>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <cstdint>

#define null NULL

#endif
