#pragma once

#include <inc.h>

#include <string_view>
#include <type_name.h>
#include <llvm/ExecutionEngine/JITSymbol.h>
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
    struct coreapi CppCompileOptions final
    {
        CppCompileOptions& withIncludeDirectories(const std::vector<std::string_view>& includeDirs);
        CppCompileOptions& withIncludes(const std::vector<std::string_view>& includeFiles);
    private:
        std::vector<std::string_view> includeDirs;
        std::vector<std::string_view> includeFiles;
    };

    class coreapi RuntimeCompiler final
    {
        static std::unique_ptr<llvm::orc::ThreadSafeContext> context;
        static std::unique_ptr<llvm::orc::KaleidoscopeJIT> jit;

        static char* evalMangled;
        
        static llvm::JITTargetAddress evalInternal(const std::string_view& code, const std::string& typeName);
        static void evalFinalize();
    public:
        RuntimeCompiler() = delete;

        static void init();

        template <typename T>
        static T evaluate(const std::string_view& code)
        {
            T ret = ((T (*)())RuntimeCompiler::evalInternal(code, gcpp::type_name<T>()))();
            RuntimeCompiler::evalFinalize();
            return ret;
        }
    };
}