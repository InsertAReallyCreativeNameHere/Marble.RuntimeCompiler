#include <iostream>
#include <RuntimeCompiler/RuntimeCompiler.h>

using namespace Marble;

int eval()
{
    return 0;
}

int main()
{
    RuntimeCompiler::init();

    std::cout << gcpp::type_name<decltype(eval)>() << std::endl;
    
    int i = RuntimeCompiler::evaluate<int>("return 420;");
    std::cout << i << std::endl;
    i = RuntimeCompiler::evaluate<long>("return 69;");
    std::cout << i << std::endl;
    i = RuntimeCompiler::evaluate<unsigned>("return 1337;");
    std::cout << i << std::endl;
    i = RuntimeCompiler::evaluate<int>
    (R"(
        unsigned long long ret = 2;
        for (int i = beg; i < end; i++)
            ret *= i;
        return ret;
    )");
    std::cout << i << std::endl;

    system("pause");
}