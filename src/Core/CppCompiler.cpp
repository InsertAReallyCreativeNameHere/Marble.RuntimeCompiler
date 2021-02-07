#include "CppCompiler.h"

#include <cc1_main.cpp>

#include <iostream>

using namespace Marble::JIT;

// "Reappropriated" from the LLVM source.
int CppCompiler::invoke(const std::string& arguments)
{
    std::vector<std::string> parsedArgs { "" };
    for (auto it = arguments.begin(); it != arguments.end(); ++it)
    {
        if (*it == ' ' || *it == '\n' || *it == '\t')
        {
            if (parsedArgs.back() != "")
                parsedArgs.push_back("");
            continue;
        }
        
        parsedArgs.back().append(&*it, 1);
    }

    for (auto it = parsedArgs.begin(); it != parsedArgs.end(); ++it)
        std::cout << *it << " ";

    std::vector<const char*> cc1args;
    cc1args.reserve(parsedArgs.size());
    for (size_t i = 0; i != cc1args.size(); i++)
    {
        cc1args[i] = &parsedArgs[i][0];
    }
    return cc1_main(cc1args, "", nullptr);
}