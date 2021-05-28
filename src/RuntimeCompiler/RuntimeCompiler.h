#pragma once

#include <inc.h>

#include <string_view>
#include <type_traits>
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
    class RuntimeCompiler;

    struct coreapi CppCompileOptions final
    {
        CppCompileOptions& withIncludeDirectories(const std::vector<std::string_view>& includeDirs);
        CppCompileOptions& withIncludes(const std::vector<std::string_view>& includeFiles);
        CppCompileOptions& withUsings(const std::vector<std::string_view>& usings);
        CppCompileOptions& usingCppStandard(const std::string_view& standard);

        friend class Marble::RuntimeCompiler;
    private:
        std::vector<std::string> includeDirs;
        std::vector<std::string> includeFiles;
        std::vector<std::string> usings;
        std::string cppStandard = "-std=c++17";
    };

    class coreapi RuntimeCompiler final
    {
        static std::unique_ptr<llvm::orc::ThreadSafeContext> context;
        static std::unique_ptr<llvm::orc::KaleidoscopeJIT> jit;

        static llvm::JITTargetAddress evalInternal(const std::string_view& code, const std::string& typeName, const CppCompileOptions& compileOptions);
        static void evalFinalize();
    public:
        RuntimeCompiler() = delete;

        static void init();

        template <typename T>
        static T evaluate(const std::string_view& code, const CppCompileOptions& compileOptions = CppCompileOptions())
        {
            if constexpr (std::is_same<T, void>::value)
            {
                ((T (*)())RuntimeCompiler::evalInternal(code, gcpp::type_name<T>(), compileOptions))();
                RuntimeCompiler::evalFinalize();
            }
            else
            {
                T ret = ((T (*)())RuntimeCompiler::evalInternal(code, gcpp::type_name<T>(), compileOptions))();
                RuntimeCompiler::evalFinalize();
                return ret;
            }
        }
    };
}