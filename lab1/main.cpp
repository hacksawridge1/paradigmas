#include <locale>
#include <codecvt>
#include <string>
#include <iostream>
#include <fstream>
#include "json.hpp"
#include "convert.h"

using std::cout;
using std::endl;
using json = nlohmann::json;

class JsonFile
{
};

int main()
{
	setlocale(LC_ALL, "");
	std::fstream f("students.json");
	json jsonData = json::parse(f);
	std::string jsonDump = jsonData.dump(2);
	std::wcout << convertAsciiToUnicode(jsonDump);
}
