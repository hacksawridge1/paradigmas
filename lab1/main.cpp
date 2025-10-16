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
private:
	const std::string filename;
	json content;

public:
	JsonFile(std::string f) : filename(f)
	{
		std::fstream fileContent(f);
		content = json::parse(fileContent);
	}

	json getRawJson()
	{
		return content;
	}

	std::string getJson()
	{
		return content.dump();
	}
};

int main()
{
	setlocale(LC_ALL, "");
	std::fstream f("students.json");
	json jsonData = json::parse(f);
	std::string jsonDump = jsonData.dump(2);
	std::wcout << convertAsciiToUnicode(jsonDump);
}
