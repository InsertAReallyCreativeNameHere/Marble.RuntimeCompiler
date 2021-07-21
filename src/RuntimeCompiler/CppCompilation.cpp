#include "CppCompilation.h"

#include <Core/CppCompiler.h>

using namespace Marble;
using namespace Marble::Internal;

CppCompilationOptions::CppCompilationOptions()
{

}

CppCompilation::CppCompilation(const std::vector<std::string_view>& sources, const CppCompilationOptions& options) :
context(std::make_unique<llvm::LLVMContext>()),
jit(std::move([] { auto jit = llvm::orc::KaleidoscopeJIT::Create(); return jit ? std::move(jit.get()) : nullptr; } ()))
{
    this->jit->addModule(llvm::orc::ThreadSafeModule(CppCompiler::invoke(sources, *this->context.getContext()), this->context));
}

llvm::JITTargetAddress CppCompilation::lookup(std::string_view symbol)
{
    auto sym = this->jit->lookup(symbol);
    return sym ? sym.get().getAddress() : 0;
}