# Marble.RuntimeCompiler
## Prerequisites:
 - LLVM built from source with clang enabled (Use -DLLVM_ENABLE_PROJECTS=clang when configuring with cmake).
 - A large quantity of patience.
 - Make sure the LLVM binaries are in your PATH.
## Instructions to build:
 - good luck ill write this later
## Future Proofing:
Steps to recreate this project if something breaks:
 - Copy llvm-project/clang/tools/driver/driver.cpp, llvm-project/clang/tools/driver/cc1_main.cpp, llvm-project/clang/tools/driver/cc1as_main.cpp, and llvm-project/clang/tools/driver/cc1gen_reproducer_main.cpp to Marble.RuntimeCompiler/clang-driver.
 - Build llvm with clang enabled.
   - Use -DLLVM_ENABLE_PROJECTS="clang" as an argument when running the cmake command.
   - Include ```json ... "cmake.configureSettings": { ... "LLVM_ENABLE_PROJECTS": "clang", ... }, ...``` in ${workspaceFolder}/.vscode/settings.json for running cmake through CMake-Tools for Visual Studio Code ("..." means other fields that could be in place).
 - 


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
