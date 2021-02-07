#include <Core/CppCompiler.h>

using namespace Marble::JIT;

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    CppCompiler::invoke("-emit-llvm -emit-llvm-bc -std=c++14 -stdlib=libc++ -resource-dir /path/to/llvm-clang-build/lib/clang/4.0.1/ -o /tmp/bitcode.bc -x c++ /tmp/source.cpp");
}