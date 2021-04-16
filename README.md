# Marble.RuntimeCompiler

## Introduction
(No dont use this yet its still trash and doesnt work). Marble.RuntimeCompiler is a (as of right now incomplete) project that aims to make clang/llvm portable. It allows for the Runtime Compilation / Just-in-Time Compilation of C/C++, through the pure power of the horcrux that is the code I have written.

## Building
 ### Prerequisites:
 - A large quantity of patience.
 - Make sure the Clang/LLVM binaries, libraries, and includes are in your PATH.
 ### Preparing and Compiling:
 - Build llvm 12.0.0 with clang enabled and the host target.
   - Include -DLLVM_ENABLE_PROJECTS="clang" -DLLVM_TARGETS_TO_BUILD="host" as arguments when running the cmake command.
   - Include ```... "cmake.configureSettings": { ..., "LLVM_ENABLE_PROJECTS": "clang", "LLVM_TARGETS_TO_BUILD": "host", ... }, ...``` in ```${workspaceFolder}/.vscode/settings.json``` for running cmake through CMake-Tools for Visual Studio Code ("..." means other JSON data).
   - I used
     ```bash
     cmake -G"MinGW Makefiles" \
     -DCMAKE_BUILD_TYPE=Release \
     -DBUILD_SHARED_LIBS=false \
     -DLLVM_ENABLE_PROJECTS="clang" \
     -DLLVM_TARGETS_TO_BUILD="host" \
     ../llvm
     ```
     to build my copy of Clang/LLVM on Windows x86_64.
 - Configure Marble.RuntimeCompiler.
 - Build Marble.RuntimeCompiler.
 - Use testexecutable.exe to test whether stuff works.

## Getting Started
 ### Basics:
 Try using Marble::RuntimeCompiler::eval<T>(const std::string_view&) to immediately run any code and get its return result. It grabs a string and runs it as if it were C++.
 ### Documentation:
 Check out the wiki!

## License
GNUGPLv3 
no its not "stealing" sources im just _reappropriating_ them.  
pls dont sue me llvm
