#pragma once

#include <inc.h>

#include <fstream>
#include <sstream>
#include <string_view>
#include <type_name.h>
#include <llvm/ExecutionEngine/JITSymbol.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/IR/LLVMContext.h>
#include <Core/CppCompiler.h>

namespace llvm
{
    namespace orc
    {
        class ThreadSafeContext;
        class KaleidoscopeJIT;
    }
}

namespace Marble
{
    namespace RuntimeCompiler
    {
        class coreapi Compiler final
        {
            static std::unique_ptr<llvm::orc::ThreadSafeContext> context;
            static std::unique_ptr<llvm::orc::KaleidoscopeJIT> jit;
            
            static llvm::JITTargetAddress evalInternal(const std::string_view& code, const std::string& typeName);
            static void evalFinalize();
        public:
            Compiler() = delete;

            static void init();

            static void evalCleanup();

            template <typename T>
            static T eval(const std::string_view& code)
            {
                T ret = ((T (*)())Compiler::evalInternal(code, gcpp::type_name<T>()))();
                Compiler::evalFinalize();
                return ret;
            }
        };
    }
}