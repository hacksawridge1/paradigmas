#ifndef CONSOLE_H
#define CONSOLE_H

#include <iostream>
#include <string_view>
#include <string>
#include "convert.h"

class Console
{
public:
  static void print(std::string_view data)
  {
    std::wcout << Convert::asciiToUtf(data) << std::endl;
  }

  static void print(std::wstring data)
  {
    std::wcout << data << std::endl;
  }
  static void print(wchar_t data)
  {
    std::wcout << data << std::endl;
  }
};

#endif