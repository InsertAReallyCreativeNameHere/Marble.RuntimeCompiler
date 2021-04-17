#include <iostream>
#include <RuntimeCompiler/RuntimeCompiler.h>

using namespace Marble::RuntimeCompiler;

int main()
{
    Compiler::init();
    
    int i = Compiler::eval<int>("return 420;");
    std::cout << i << std::endl;
    i = Compiler::eval<int>("return 69;");
    std::cout << i << std::endl;
    i = Compiler::eval<int>("return 1337;");
    std::cout << i << std::endl;
    i = Compiler::eval<int>("weed");
    std::cout << i << std::endl;
    system("pause");
    i = Compiler::eval<int>
    (R"(
        int ret = 0;
        for (int i = 0; i < 10; i++)
            ret += i;
        return ret;
    )");
    std::cout << i << std::endl;

    system("pause");
}