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

# Begin modifiable options.

set(CMAKE_BUILD_TYPE Debug)

# End modifiable options.

project(Marble.RuntimeCompiler)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

set(CMAKE_SHARED_LIBRARY_PREFIX "")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic-errors") # Don't forget, put -flto back when done with making this library.
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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=lld -flto -pthread")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    endif()
    set(COMPILER "CLang")
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

target_compile_definitions(Marble.RuntimeCompiler PUBLIC "BUILD_TYPE_DYNAMIC=1")
if (${BUILD_PLATFORM} STREQUAL Win32)
    execute_process(COMMAND CMD /c llvm-config --libs OUTPUT_VARIABLE LLVM_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)
    target_link_libraries(Marble.RuntimeCompiler PUBLIC
        ${LLVM_LIBRARIES}
        clangFrontendTool
    )
    target_include_directories(Marble.RuntimeCompiler PUBLIC
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/cc1"
        "${LLVM_INCLUDE_DIRS}"
    )
endif()
message("LLVM required libraries found: ${LLVM_LIBRARIES}")

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