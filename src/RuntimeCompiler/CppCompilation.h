#pragma once

#include "inc.h"

#include <llvm/ExecutionEngine/JitSymbol.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <KaleidoscopeJIT.h>

namespace Marble
{
    struct coreapi CppCompilationOptions final
    {
        CppCompilationOptions();
    };

    class coreapi CppCompilation final
    {
        llvm::orc::ThreadSafeContext context;
        std::unique_ptr<llvm::orc::KaleidoscopeJIT> jit;
    public:
        CppCompilation(const std::vector<std::string_view>& sources, const CppCompilationOptions& options = CppCompilationOptions());

        llvm::JITTargetAddress lookup(std::string_view symbol);
    };
}