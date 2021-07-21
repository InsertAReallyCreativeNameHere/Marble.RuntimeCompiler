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

    int i = RuntimeCompiler::evaluate<int>("return 420;");
    std::cout << i << '\n';
    i = RuntimeCompiler::evaluate<int>("return 69;");
    std::cout << i << '\n';

    system("pause");
}
