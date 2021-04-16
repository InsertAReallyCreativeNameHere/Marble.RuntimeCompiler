#pragma once

#include <inc.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string_view>
#include <type_name.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/TargetRegistry.h>
#include <KaleidoscopeJIT.h>
#include <Core/CppCompiler.h>

namespace Marble
{
    namespace RuntimeCompiler
    {
        class coreapi Compiler final
        {
            static std::unique_ptr<llvm::LLVMContext> context;
        public:
            Compiler() = delete;

            static void init();
            static void fin();

            template <typename T>
            static T eval(const std::string_view& code)
            {
                std::ostringstream compileCode;
                compileCode <<
                gcpp::type_name<T>() <<
                " eval()\n{\n" <<
                code.begin() <<
                "\n}";

                if (!std::filesystem::exists("RuntimeInternal/"))
                    std::filesystem::create_directory("RuntimeInternal/");
                
                std::ofstream file("RuntimeInternal/eval.cpp");
                file << compileCode.str();
                file.close();

                Internal::CppCompiler::invoke
                ({
                    "-cc1", "-emit-llvm", "-emit-llvm-bc", "-std=c++17",
                    "-o", "RuntimeInternal/eval.bc", "-x", "c++",
                    "RuntimeInternal/eval.cpp"
                });

                std::string lkerr;
                llvm::TargetRegistry::lookupTarget("i686-pc-windows-gnu", lkerr);
                puts(lkerr.c_str());

                std::unique_ptr<llvm::orc::KaleidoscopeJIT> jit = std::move(llvm::orc::KaleidoscopeJIT::Create().get());

                std::unique_ptr<llvm::MemoryBuffer> bitcodeBuffer = std::move(llvm::MemoryBuffer::getFile("RuntimeInternal/eval.bc").get());
                std::unique_ptr<llvm::Module> evalModule = std::move(llvm::parseBitcodeFile(bitcodeBuffer->getMemBufferRef(), *Compiler::context).get());
                evalModule->setDataLayout(jit->getDataLayout());
                evalModule->print(llvm::errs(), nullptr);

                llvm::Error err = jit->addModule(llvm::orc::ThreadSafeModule(std::move(evalModule), std::move(Compiler::context)));

                auto symbol = jit->lookup("eval");
                T (*evalFuncCompiled)() = (decltype(evalFuncCompiled))symbol.get().getAddress();
                return evalFuncCompiled();
            }
        };
    }
}