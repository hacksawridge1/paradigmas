#include <iostream>
#include <string>

class Console
{
    public:
        void print(std::string data)
        {
            std::cout << data << std::endl;
        }
        void print(std::wstring data)
        {
            std::wcout << data;
        }
};