#include <string>
#include <iostream>
#include <codecvt>
#include <locale>

std::wstring convertAsciiToUnicode(std::string str)
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  return converter.from_bytes(str);
}