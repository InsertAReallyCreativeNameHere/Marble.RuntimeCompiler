cmake_minimum_required(VERSION 3.10)

project(Marble.RuntimeCompiler)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BUILD_ARCH x64)
else()
    set(BUILD_ARCH x86)
endif()

if ((NOT DEFINED CMAKE_BUILD_TYPE) OR (CMAKE_BUILD_TYPE STREQUAL ""))
    set(CMAKE_BUILD_TYPE "Release")
endif()
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-D_DEBUG=1)
endif()

if (WIN32 OR WIN64)
    set(BUILD_PLATFORM Win32)
elseif (UNIX AND NOT APPLE)
    set(BUILD_PLATFORM Linux)
elseif (APPLE)
    set(BUILD_PLATFORM MacOS)
else()
    set(BUILD_PLATFORM "UnknownPlatform")
endif()

set(CMAKE_CXX_STANDARD 20)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (CMAKE_SHARED_LIBRARY_SUFFIX STREQUAL ".dll")
        set(COMPILER "MinGW")
    else()
        set(COMPILER "gcc")
    endif()
    set(BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE INTERNAL "Configuration string in output dir." FORCE)
    set(OUTNAME "${BUILD_ARCH}-${COMPILER}-${BUILD_TYPE}-${BUILD_PLATFORM}")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(COMPILER "Clang")
    set(BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE INTERNAL "Configuration string in output dir." FORCE)
    set(OUTNAME "${BUILD_ARCH}-${COMPILER}-${BUILD_TYPE}-${BUILD_PLATFORM}")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(COMPILER "MSVC")
    set(BUILD_TYPE "UnknownBuildType")
    set(OUTNAME "${BUILD_ARCH}-${COMPILER}-${BUILD_PLATFORM}")
else()
    set(COMPILER ${CMAKE_CXX_COMPILER_ID})
    set(BUILD_TYPE "UnknownBuildType")
    set(OUTNAME "${BUILD_ARCH}-Unknown-${BUILD_PLATFORM}")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

# llvm-project
set(_CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE})
set(CMAKE_BUILD_TYPE "Release") # Builds faster for MinGW/GCC this way.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../lib/${OUTNAME}/llvm-project)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../lib/${OUTNAME}/llvm-project)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin/${OUTNAME}/llvm-project)
set(LLVM_ENABLE_PROJECTS "clang" CACHE STRING "LLVM projects to build." FORCE)
set(LLVM_TARGETS_TO_BUILD "host" CACHE STRING "LLVM targets to build." FORCE)
add_subdirectory("${CMAKE_SOURCE_DIR}/external/llvm-project/llvm")
set(CMAKE_BUILD_TYPE ${_CMAKE_BUILD_TYPE})

# Marble.RuntimeCompiler

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../lib/${OUTNAME}/Marble.RuntimeCompiler)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../lib/${OUTNAME}/Marble.RuntimeCompiler)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin/${OUTNAME}/Marble.RuntimeCompiler)

file(GLOB_RECURSE CMPSRCS configure_depends
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_library(Marble.RuntimeCompiler SHARED ${CMPSRCS})

target_compile_definitions(Marble.RuntimeCompiler PUBLIC "BUILD_TYPE_DYNAMIC=1")
if (${BUILD_PLATFORM} STREQUAL Win32)
    target_link_libraries(Marble.RuntimeCompiler PUBLIC
        clangFrontendTool

        LLVMWindowsManifest
        LLVMXRay
        LLVMLibDriver
        LLVMDlltoolDriver
        LLVMCoverage
        LLVMLineEditor
        LLVMX86Disassembler
        LLVMX86AsmParser
        LLVMX86CodeGen
        LLVMX86Desc
        LLVMX86Info
        LLVMOrcJIT
        LLVMMCJIT
        LLVMJITLink
        LLVMOrcTargetProcess
        LLVMOrcShared
        LLVMInterpreter
        LLVMExecutionEngine
        LLVMRuntimeDyld
        LLVMSymbolize
        LLVMDebugInfoPDB
        LLVMDebugInfoGSYM
        LLVMOption
        LLVMObjectYAML
        LLVMMCA
        LLVMMCDisassembler
        LLVMLTO
        LLVMPasses
        LLVMCFGuard
        LLVMCoroutines
        LLVMObjCARCOpts
        LLVMipo
        LLVMVectorize
        LLVMLinker
        LLVMInstrumentation
        LLVMFrontendOpenMP
        LLVMFrontendOpenACC
        LLVMExtensions
        LLVMDWARFLinker
        LLVMGlobalISel
        LLVMMIRParser
        LLVMAsmPrinter
        LLVMDebugInfoDWARF
        LLVMSelectionDAG
        LLVMCodeGen
        LLVMIRReader
        LLVMAsmParser
        LLVMInterfaceStub
        LLVMFileCheck
        LLVMFuzzMutate
        LLVMTarget
        LLVMScalarOpts
        LLVMInstCombine
        LLVMAggressiveInstCombine
        LLVMTransformUtils
        LLVMBitWriter
        LLVMAnalysis
        LLVMProfileData
        LLVMObject
        LLVMTextAPI
        LLVMMCParser
        LLVMMC
        LLVMDebugInfoCodeView
        LLVMDebugInfoMSF
        LLVMBitReader
        LLVMCore
        LLVMRemarks
        LLVMBitstreamReader
        LLVMBinaryFormat
        LLVMTableGen
        LLVMSupport
        LLVMDemangle
    )
    target_include_directories(Marble.RuntimeCompiler PUBLIC
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/clang-driver"
        "${CMAKE_SOURCE_DIR}/dependencies/gcpp/include"
        "${CMAKE_SOURCE_DIR}/dependencies/Kaleidoscope"
        "${LLVM_INCLUDE_DIRS}"
    )
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../lib/${OUTNAME}/testexecutable)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../lib/${OUTNAME}/testexecutable)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin/${OUTNAME}/testexecutable)

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
    "${CMAKE_SOURCE_DIR}/bin/${OUTNAME}/Marble.RuntimeCompiler/Marble.RuntimeCompiler.dll"
    $<TARGET_FILE_DIR:testexecutable>
)