#include "RuntimeCompiler.h"

#include <filesystem>
#include <KaleidoscopeJIT.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JITLink/JITLink.h>
#include <llvm/ExecutionEngine/JITSymbol.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/RTDyldMemoryManager.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Mangler.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>

using namespace Marble::Internal;
using namespace Marble::RuntimeCompiler;
namespace fs = std::filesystem;

std::unique_ptr<llvm::orc::ThreadSafeContext> Compiler::context;
std::unique_ptr<llvm::orc::KaleidoscopeJIT> Compiler::jit;

void Compiler::recreateJIT()
{
    Compiler::jit.reset();
    Compiler::jit = std::move(llvm::orc::KaleidoscopeJIT::Create().get());
}

void Compiler::init()
{
    if (llvm::InitializeNativeTarget() == 1)
        puts("fricc");
    if (llvm::InitializeNativeTargetAsmPrinter() == 1)
        puts("fricc");
    if (llvm::InitializeNativeTargetAsmParser() == 1)
        puts("fricc");
    
    Compiler::context = std::make_unique<llvm::orc::ThreadSafeContext>(std::make_unique<llvm::LLVMContext>());
    Compiler::jit = std::move(llvm::orc::KaleidoscopeJIT::Create().get());
}

void* Compiler::evalInternal(const std::string& compileCode)
{
    if (!std::filesystem::exists("RuntimeInternal/"))
        std::filesystem::create_directory("RuntimeInternal/");
    
    std::ofstream file("RuntimeInternal/eval.cpp");
    file << compileCode;
    file.close();

    Internal::CppCompiler::invoke
    ({
        "-cc1", "-emit-llvm", "-emit-llvm-bc", "-std=c++17",
        "-o", "RuntimeInternal/eval.bc", "-x", "c++",
        "RuntimeInternal/eval.cpp"
    });
    
    std::unique_ptr<llvm::MemoryBuffer> bitcodeBuffer = std::move(llvm::MemoryBuffer::getFile("RuntimeInternal/eval.bc").get());
    std::unique_ptr<llvm::Module> evalModule = std::move(llvm::parseBitcodeFile(bitcodeBuffer->getMemBufferRef(), *Compiler::context->getContext()).get());
    //evalModule->setDataLayout(Compiler::jit->getDataLayout());

    llvm::orc::ThreadSafeModule mod(std::move(evalModule), *Compiler::context);
    llvm::Error err = Compiler::jit->addModule(std::move(mod));

    return (void*)Compiler::jit->lookup("_Z4evalv")->getAddress();
}