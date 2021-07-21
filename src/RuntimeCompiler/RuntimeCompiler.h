#pragma once

#include <inc.h>

#include <sstream>
#include <string_view>
#include <type_traits>
#include <type_name.h>
#include <llvm/ExecutionEngine/JITSymbol.h>
#include <Core/CppCompiler.h>
#include <RuntimeCompiler/CppCompilation.h>

namespace Marble
{
    class RuntimeCompiler;
/*
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
*/
    class coreapi RuntimeCompiler final
    {
        static llvm::JITTargetAddress evalInternal(std::string typeName, std::string_view code);
    public:
        RuntimeCompiler() = delete;

        static void init();

        template <typename T>
        inline static T evaluate(std::string_view code, const CppCompilationOptions& compileOptions = CppCompilationOptions())
        {
            /*for (auto it = compileOptions.includeFiles.begin(); it != compileOptions.includeFiles.end(); ++it)
                compileCode << *it << "\n";
            for (auto it = compileOptions.usings.begin(); it != compileOptions.usings.end(); ++it)
                compileCode << *it << "\n";*/
            std::string compileCode(std::move(gcpp::type_name<T>()));
            compileCode
            .append(" eval()\n{\n")
            .append(code.data())
            .append("\n}");

            if constexpr (std::is_same<T, void>::value)
                ((T (*)())CppCompilation({ compileCode.c_str() }).lookup("eval"))();
            else return ((T (*)())CppCompilation({ compileCode.c_str() }).lookup("eval"))();
        }
    };
}