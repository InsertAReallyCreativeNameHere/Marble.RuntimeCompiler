#include "RuntimeCompiler.h"

#include <filesystem>
#include <fstream>
#include <clang/AST/ASTContext.h>
#include <clang/AST/Mangle.h>
#include <clang/Frontend/CompilerInstance.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Support/TargetSelect.h>
#include <KaleidoscopeJIT.h>

using namespace Marble;
using namespace Marble::Internal;
namespace fs = std::filesystem;

CppCompileOptions& CppCompileOptions::withIncludeDirectories(const std::vector<std::string_view>& includeDirs)
{
    this->includeDirs.reserve(includeDirs.size() * 2);
    for (auto it = includeDirs.begin(); it != includeDirs.end(); ++it)
    {
        this->includeDirs.push_back("-internal-isystem");
        this->includeDirs.push_back((*it).begin());
    }
    
    return *this;
}
CppCompileOptions& CppCompileOptions::withIncludes(const std::vector<std::string_view>& includeFiles)
{
    this->includeFiles.reserve(includeFiles.size());
    for (auto it = includeFiles.begin(); it != includeFiles.end(); ++it)
    {
        std::string str;
        str.reserve((*it).size() + 11);
        str.append("#include \"");
        str.append((*it).begin());
        str.append("\"");
        this->includeFiles.push_back(std::move(str));
    }
    
    return *this;
}
CppCompileOptions& CppCompileOptions::withUsings(const std::vector<std::string_view>& usings)
{
    this->usings.reserve(usings.size());
    for (auto it = usings.begin(); it != usings.end(); ++it)
    {
        std::string str;
        str.reserve((*it).size() + 7);
        str.append("using ");
        str.append((*it).begin());
        str.append(";");
        this->usings.push_back(std::move(str));
    }
    
    return *this;
}
CppCompileOptions& CppCompileOptions::usingCppStandard(const std::string_view& standard)
{
    this->cppStandard.clear();
    this->cppStandard.reserve(standard.size() + 5);
    this->cppStandard.append("-std=");
    this->cppStandard.append(standard.begin());
    
    return *this;
}

std::unique_ptr<llvm::orc::ThreadSafeContext> RuntimeCompiler::context;
std::unique_ptr<llvm::orc::KaleidoscopeJIT> RuntimeCompiler::jit;

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

llvm::JITTargetAddress RuntimeCompiler::evalInternal(const std::string_view& code, const std::string& typeName, const CppCompileOptions& compileOptions)
{
    std::ostringstream compileCode;
    for (auto it = compileOptions.includeFiles.begin(); it != compileOptions.includeFiles.end(); ++it)
        compileCode << *it << "\n";
    for (auto it = compileOptions.usings.begin(); it != compileOptions.usings.end(); ++it)
        compileCode << *it << "\n";
    compileCode <<
    "void puts(const char*);"
    "\n" <<
    typeName <<
    " eval()\n{\n" <<
    code.begin() <<
    "\n}";

    if (!std::filesystem::exists("RuntimeInternal/"))
        std::filesystem::create_directory("RuntimeInternal/");
    
    std::ofstream file("RuntimeInternal/eval.cpp");
    file << compileCode.str();
    file.close();

    std::vector<const char*> args
    ({
        "-emit-llvm",
        "-emit-llvm-bc",
        compileOptions.cppStandard.c_str()
    });
    for (auto it = compileOptions.includeDirs.begin(); it != compileOptions.includeDirs.end(); ++it)
        args.push_back((*it).c_str());
    args.insert
    (
        args.end(),
        {
            "-resource-dir",
            "D:\\Github\\CykaBlyat-Tools\\mingw32\\lib\\clang\\12.0.0",
            "-o", "RuntimeInternal/eval.bc",
            "-x", "c++",
            "RuntimeInternal/eval.cpp"
        }
    );
    Internal::CppCompiler::invoke(args);

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