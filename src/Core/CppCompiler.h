#pragma once

#include <inc.h>

#include <vector>

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