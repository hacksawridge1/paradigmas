#ifndef CONVERT_H
#define CONVERT_H

#include <string>
#include <iostream>
#include <codecvt>
#include <locale>
#include <variant>
#include <string_view>

std::wstring asciiToUtf(std::string_view str)
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  std::wstring wideJson = converter.from_bytes(std::string(str));
  return wideJson;
}

std::wstring asciiToUtf(const char ch)
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  std::wstring wideJson = converter.from_bytes(ch);
  return wideJson;
}

#endif