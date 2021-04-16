#pragma once

#include <inc.h>

namespace Marble
{
    namespace Internal
    {
        class coreapi CppCompiler final
        {
        public:
            CppCompiler() = delete;

            static int invoke(const std::vector<const char*>& args);
        };
    }
}