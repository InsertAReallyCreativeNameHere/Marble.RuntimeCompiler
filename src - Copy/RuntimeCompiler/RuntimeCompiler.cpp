#include "RuntimeCompiler.h"

#include <llvm/Support/TargetSelect.h>

using namespace Marble::Internal;
using namespace Marble::RuntimeCompiler;
namespace fs = std::filesystem;

std::unique_ptr<llvm::LLVMContext> Compiler::context;

void Compiler::init()
{
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllDisassemblers();
    llvm::InitializeAllAsmPrinters();
    llvm::InitializeAllAsmParsers();

    Compiler::context = std::make_unique<llvm::LLVMContext>();
}
void Compiler::fin()
{
}