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
    RuntimeCompiler::evaluate<void>
    (
        R"(
            cout << "wow" << endl;
        )",
        CppCompileOptions()
        .withIncludeDirectories
        ({
            "D:\\Github\\CykaBlyat-Tools\\mingw32\\i686-w64-mingw32\\include\\c++\\i686-w64-mingw32",
            "D:\\Github\\CykaBlyat-Tools\\mingw32\\i686-w64-mingw32\\include\\c++\\backward",
            "D:\\Github\\CykaBlyat-Tools\\mingw32\\i686-w64-mingw32\\include\\c++\\10.3.0",
            "D:\\Github\\CykaBlyat-Tools\\mingw32\\i686-w64-mingw32\\include\\c++\\10.3.0\\i686-w64-mingw32",
            "D:\\Github\\CykaBlyat-Tools\\mingw32\\i686-w64-mingw32\\include\\c++\\10.3.0\\backward",
            "D:\\Github\\CykaBlyat-Tools\\mingw32\\include\\c++\\10.3.0",
            "D:\\Github\\CykaBlyat-Tools\\mingw32\\include\\c++\\10.3.0\\i686-w64-mingw32",
            "D:\\Github\\CykaBlyat-Tools\\mingw32\\include\\c++\\10.3.0\\backward",
            "D:\\Github\\CykaBlyat-Tools\\mingw32\\lib\\gcc\\i686-w64-mingw32\\10.3.0\\include\\c++",
            "D:\\Github\\CykaBlyat-Tools\\mingw32\\lib\\gcc\\i686-w64-mingw32\\10.3.0\\include\\c++\\i686-w64-mingw32",
            "D:\\Github\\CykaBlyat-Tools\\mingw32\\lib\\gcc\\i686-w64-mingw32\\10.3.0\\include\\c++\\backward",
            "D:\\Github\\CykaBlyat-Tools\\mingw32\\lib\\clang\\12.0.0\\include",
            "D:\\Github\\CykaBlyat-Tools\\mingw32\\i686-w64-mingw32/sys-root/mingw/include",
            "D:\\Github\\CykaBlyat-Tools\\mingw32\\i686-w64-mingw32\\include",
            "D:\\Github\\CykaBlyat-Tools\\mingw32\\include"
        })
        .withIncludes({ "iostream" })
        .withUsings({ "namespace std" })
    );

    system("pause");
}