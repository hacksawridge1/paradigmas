#include <string>
#include <iostream>
#include <codecvt>
#include <locale>

class Convert
{
    public:
        std::wstring asciiToUtf(std::string str)
        {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            std::wstring wideJson = converter.from_bytes(str);
            return wideJson;
        }
};