#include "RuntimeCompiler.h"

#include <filesystem>
#include <fstream>
#include <clang/AST/ASTContext.h>
#include <clang/AST/Mangle.h>
#include <clang/Frontend/CompilerInstance.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Support/TargetSelect.h>
#include <KaleidoscopeJIT.h>

using namespace Marble;
using namespace Marble::Internal;
namespace fs = std::filesystem;

std::unique_ptr<llvm::orc::ThreadSafeContext> RuntimeCompiler::context;
std::unique_ptr<llvm::orc::KaleidoscopeJIT> RuntimeCompiler::jit;

char* RuntimeCompiler::evalMangled;

void RuntimeCompiler::init()
{
    if (llvm::InitializeNativeTarget() == 1)
        puts("fricc");
    if (llvm::InitializeNativeTargetAsmPrinter() == 1)
        puts("fricc");
    if (llvm::InitializeNativeTargetAsmParser() == 1)
        puts("fricc");
    
    RuntimeCompiler::context = std::make_unique<llvm::orc::ThreadSafeContext>(std::make_unique<llvm::LLVMContext>());
}

llvm::JITTargetAddress RuntimeCompiler::evalInternal(const std::string_view& code, const std::string& typeName)
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
    
    RuntimeCompiler::jit = std::move(llvm::orc::KaleidoscopeJIT::Create().get());
    std::unique_ptr<llvm::Module> evalModule = std::move(llvm::parseBitcodeFile
    (
        llvm::MemoryBuffer::getFile("RuntimeInternal/eval.bc").get()->getMemBufferRef(),
        *RuntimeCompiler::context->getContext()
    )
    .get());
    llvm::StringRef evalFuncName = evalModule->getFunctionList().begin()->getName();
    llvm::Error err = RuntimeCompiler::jit->addModule({ std::move(evalModule), *RuntimeCompiler::context });

    return RuntimeCompiler::jit->lookup(evalFuncName)->getAddress();
}
void RuntimeCompiler::evalFinalize()
{
    RuntimeCompiler::jit.reset();
}