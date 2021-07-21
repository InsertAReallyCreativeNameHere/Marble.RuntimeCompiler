#pragma once

#include <inc.h>

#include <clang/Basic/LangStandard.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/IR/LLVMContext.h>
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
            static void init();
            static void deinit();
        public:
            CppCompiler() = delete;

            static std::unique_ptr<llvm::Module> invoke(const std::vector<std::string_view>& sources, llvm::LLVMContext& context);

            friend class Marble::RuntimeCompiler;
        };
    }
}