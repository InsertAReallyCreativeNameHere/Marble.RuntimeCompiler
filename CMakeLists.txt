cmake_minimum_required(VERSION 3.10)

if (WIN32)
    set(BUILD_PLATFORM Win32)
elseif (UNIX AND NOT APPLE)
    set(BUILD_PLATFORM Linux)
else()
    set(BUILD_PLATFORM MacOS)
endif()

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BUILD_ARCH x64)
else()
    set(BUILD_ARCH x86)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Marble.RuntimeCompiler)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

set(CMAKE_SHARED_LIBRARY_PREFIX "")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic-errors -std=c++17")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    endif()
    if (BUILD_PLATFORM STREQUAL "Win32")
        set(COMPILER "MinGW")
    elseif()
        set(COMPILER "gcc")
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=lld -pthread -std=c++17")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    endif()
    set(COMPILER "Clang")
else()
    message(FATAL_ERROR "No supported compiler used.")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../lib/${BUILD_ARCH}-${COMPILER}-${CMAKE_BUILD_TYPE}-${BUILD_PLATFORM}/Marble.RuntimeCompiler)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../lib/${BUILD_ARCH}-${COMPILER}-${CMAKE_BUILD_TYPE}-${BUILD_PLATFORM}/Marble.RuntimeCompiler)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin/${BUILD_ARCH}-${COMPILER}-${CMAKE_BUILD_TYPE}-${BUILD_PLATFORM}/Marble.RuntimeCompiler)

file(GLOB_RECURSE CMPSRCS configure_depends
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_library(Marble.RuntimeCompiler SHARED ${CMPSRCS})

execute_process(COMMAND llvm-config --libs OUTPUT_VARIABLE LIBS)
execute_process(COMMAND llvm-config --system-libs OUTPUT_VARIABLE SYS_LIBS)
execute_process(COMMAND llvm-config --ldflags OUTPUT_VARIABLE LDF)
string(STRIP ${LIBS} LIBS)
string(STRIP ${SYS_LIBS} SYS_LIBS)
string(STRIP ${LDF} LDF)
message(STATUS "Found LLVM: " ${LIBS})

target_compile_definitions(Marble.RuntimeCompiler PUBLIC "BUILD_TYPE_DYNAMIC=1")
if (${BUILD_PLATFORM} STREQUAL Win32)
    target_link_libraries(Marble.RuntimeCompiler PUBLIC
        ${LIBS} ${SYS_LIBS} ${LDF}
        clangFrontendTool
    )
    target_include_directories(Marble.RuntimeCompiler PUBLIC
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/clang-driver"
        "${CMAKE_SOURCE_DIR}/dependencies/gcpp/include"
        "${CMAKE_SOURCE_DIR}/dependencies/Kaleidoscope"
        "${LLVM_INCLUDE_DIRS}"
    )
endif()

project(testexecutable)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../lib/${BUILD_ARCH}-${COMPILER}-${CMAKE_BUILD_TYPE}-${BUILD_PLATFORM}/testexecutable)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../lib/${BUILD_ARCH}-${COMPILER}-${CMAKE_BUILD_TYPE}-${BUILD_PLATFORM}/testexecutable)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin/${BUILD_ARCH}-${COMPILER}-${CMAKE_BUILD_TYPE}-${BUILD_PLATFORM}/testexecutable)

file(GLOB_RECURSE TESTSRCS configure_depends
    "${CMAKE_SOURCE_DIR}/testexecutable/*.cpp"
)

add_executable(testexecutable ${TESTSRCS})

if (${BUILD_PLATFORM} STREQUAL Win32)
    target_link_libraries(testexecutable PUBLIC
        Marble.RuntimeCompiler
    )
    target_include_directories(testexecutable PUBLIC
    )
endif()

add_custom_command(TARGET testexecutable POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/bin/${BUILD_ARCH}-${COMPILER}-${CMAKE_BUILD_TYPE}-${BUILD_PLATFORM}/Marble.RuntimeCompiler/Marble.RuntimeCompiler.dll"
    $<TARGET_FILE_DIR:testexecutable>
)