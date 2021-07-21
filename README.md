# Marble.RuntimeCompiler

## Introduction
Marble.RuntimeCompiler is a project that aims to make C++ JIT'able. It allows for the Runtime Compilation / Just-in-Time Compilation of C/C++, through clang/LLVM.

## Building
 ### Prerequisites:
 - A large quantity of patience.
 - Have a supported compiler: MinGW, MSVC, (probably) Clang.
 ### Preparing and Compiling:
 On the commandline:
 \[In the root of this project, where CMakeLists.txt is.\]
 ```bash
 mkdir build
 cd build
 cmake ..
 [whatever build command here i dunno i use vscode]
 ```
 In VSCode:
  - Configure Marble.RuntimeCompiler.
  - Build Marble.RuntimeCompiler.

## Getting Started
 ### Basics:
 Try using ```Marble::RuntimeCompiler::evaluate<T>(std::string_view code, const CppCompilationOptions& compileOptions = CppCompilationOptions())``` to immediately run any code and get its return result. It grabs a string and runs it as if it were C++.
 ### Documentation:
 Check out the wiki!

## License
Attribution-NonCommercial-ShareAlike 4.0
