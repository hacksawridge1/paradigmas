#ifndef CONSOLE_H
#define CONSOLE_H

#include <iostream>
#include <string_view>
#include <string>
#include "convert.h"

void print(std::string_view data)
{
  std::wcout << asciiToUtf(data) << std::endl;
}

void print(std::wstring data)
{
  std::wcout << data << std::endl;
}

void print(wchar_t data)
{
  std::wcout << data << std::endl;
}

#endif