#pragma once

#include <inc.h>

#include <fstream>
#include <sstream>
#include <string_view>
#include <type_name.h>
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
            
            static void recreateJIT();
        public:
            Compiler() = delete;

            static void init();

            static void* evalInternal(const std::string& compileCode);
            static void evalCleanup();

            template <typename T>
            static T eval(const std::string_view& code)
            {
                std::ostringstream compileCode;
                compileCode <<
                gcpp::type_name<T>() <<
                " eval()\n{\n" <<
                code.begin() <<
                "\n}";

                T ret = ((T (*)())Compiler::evalInternal(compileCode.str()))();
                Compiler::recreateJIT();
                return ret;
            }
        };
    }
}