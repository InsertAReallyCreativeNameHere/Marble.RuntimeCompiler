#include "RuntimeCompiler.h"

#include <filesystem>
#include <KaleidoscopeJIT.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Support/TargetSelect.h>

using namespace Marble::Internal;
using namespace Marble::RuntimeCompiler;
namespace fs = std::filesystem;

std::unique_ptr<llvm::orc::ThreadSafeContext> Compiler::context;
std::unique_ptr<llvm::orc::KaleidoscopeJIT> Compiler::jit;

void Compiler::init()
{
    if (llvm::InitializeNativeTarget() == 1)
        puts("fricc");
    if (llvm::InitializeNativeTargetAsmPrinter() == 1)
        puts("fricc");
    if (llvm::InitializeNativeTargetAsmParser() == 1)
        puts("fricc");
    
    Compiler::context = std::make_unique<llvm::orc::ThreadSafeContext>(std::make_unique<llvm::LLVMContext>());
}

llvm::JITTargetAddress Compiler::evalInternal(const std::string_view& code, const std::string& typeName)
{
    std::ostringstream compileCode;
    compileCode <<
    typeName <<
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
    
    Compiler::jit = std::move(llvm::orc::KaleidoscopeJIT::Create().get());
    std::unique_ptr<llvm::Module> evalModule = std::move(llvm::parseBitcodeFile(llvm::MemoryBuffer::getFile("RuntimeInternal/eval.bc").get()->getMemBufferRef(), *Compiler::context->getContext()).get());
    llvm::Error err = Compiler::jit->addModule({ std::move(evalModule), *Compiler::context });

    return Compiler::jit->lookup("_Z4evalv")->getAddress();
}
void Compiler::evalFinalize()
{
    Compiler::jit.reset();
}