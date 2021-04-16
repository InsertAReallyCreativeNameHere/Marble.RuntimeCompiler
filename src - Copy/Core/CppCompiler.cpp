#include "CppCompiler.h"

#include <clang/Driver/ToolChain.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/StringSaver.h>

using namespace llvm;
using namespace clang::driver;
using namespace Marble::Internal;

extern int cc1_main(ArrayRef<const char *> Argv, const char *Argv0, void *MainAddr);

int CppCompiler::invoke(const std::vector<const char*>& args)
{
    return cc1_main(llvm::makeArrayRef(args), "69 funi", nullptr);
}