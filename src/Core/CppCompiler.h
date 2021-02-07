#pragma once

#include <inc.h>

namespace Marble
{
    namespace JIT
    {
        class coreapi CppCompiler final
        {
        public:
            CppCompiler() = delete;

            static int invoke(const std::string& arguments);
        };
    }
}