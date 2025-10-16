#include <locale>
#include <codecvt>
#include <string>
#include <iostream>
#include <fstream>
#include "lib/json.hpp"
#include "lib/convert.cpp"
#include "lib/console.cpp"
#include "lib/jsonfile.cpp"

bool isFirstLetterA(const std::string& str);

int main()
{
	setlocale(LC_ALL, "");
	Convert converter;
	JsonFile json("students.json");
	Console con;
	nlohmann::json sortedJson;
	for (auto& el : json.getRawJson()["students"])
	{
		std::cout << el << "\n";
		std::cout << isFirstLetterA(el["name"]) << "\n";
		if (isFirstLetterA(el["name"]))
		{
			sortedJson.push_back(el);
		}
	};
	return 0;
}

bool isFirstLetterA(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    return str[0] == 'A' || str[0] == 'a';
}
