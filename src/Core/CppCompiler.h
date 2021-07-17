#pragma once

#include <inc.h>

#include <clang/Basic/LangStandard.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/MemoryBufferRef.h>
#include <vector>

namespace Marble
{
    class RuntimeCompiler;

    namespace Internal
    {
        class coreapi CppCompiler final
        {
            static std::unique_ptr<llvm::orc::ThreadSafeContext> context;

            static void init();
            static void deinit();
        public:
            CppCompiler() = delete;

            static std::unique_ptr<llvm::Module> invoke(const std::vector<const char*>& args, const char* code, clang::Language lang = clang::Language::CXX);

            friend class Marble::RuntimeCompiler;
        };
    }
}