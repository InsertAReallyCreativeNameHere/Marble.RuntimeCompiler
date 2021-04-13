# Marble.RuntimeCompiler

## Introduction
Marble.RuntimeCompiler is a (as of right now incomplete) project that aims to make clang/llvm portable. It allows for the Runtime Compilation / Just-in-Time Compilation of C/C++, through the pure energy from my suffering that is ingrained through the code I have written.

## Building
 ### Prerequisites:
 - A large quantity of patience.
 - Make sure the LLVM binaries are in your PATH.
 ### Preparing and Compiling:
 - Copy ```llvm-project/clang/tools/driver/driver.cpp```, ```llvm-project/clang/tools/driver/cc1_main.cpp```, ```llvm-project/clang/tools/driver/cc1as_main.cpp```, and ```llvm-project/clang/tools/driver/cc1gen_reproducer_main.cpp``` to ```Marble.RuntimeCompiler/clang-driver```.
 - Modify the llvm/clang source.
   - Remove
     ```cpp
     #ifdef _WIN32
     // We use UTF-8 as the internal character encoding. On Windows,
     // arguments passed to main() may not be encoded in UTF-8. In order
     // to reliably detect encoding of command line arguments, we use an
     // Windows API to obtain arguments, convert them to UTF-8, and then
     // write them back to the Argv vector.
     //
     // There's probably other way to do the same thing (e.g. using
     // wmain() instead of main()), but this way seems less intrusive
     // than that.
     std::string Banner = std::string(Argv[0]) + ": ";
     ExitOnError ExitOnErr(Banner);

     ExitOnErr(errorCodeToError(windows::GetCommandLineArguments(Args, Alloc)));

     // GetCommandLineArguments doesn't terminate the vector with a
     // nullptr.  Do it to make it compatible with the real argv.
     Args.push_back(nullptr);

     Argc = Args.size() - 1;
     Argv = Args.data();
     #endif
     ```
     from ```llvm-project/llvm/lib/Support/InitLLVM.cpp``` (in the constructor).
 - Build llvm with clang enabled.
   - Use -DLLVM_ENABLE_PROJECTS="clang" as an argument when running the cmake command.
   - Include ```... "cmake.configureSettings": { ... "LLVM_ENABLE_PROJECTS": "clang", ... }, ...``` in ```${workspaceFolder}/.vscode/settings.json``` for running cmake through CMake-Tools for Visual Studio Code ("..." means other fields).

## 
pls dont sue me llvm
