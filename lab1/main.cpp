#include <iostream>
#include <string>
#include <fstream>
#include "./json.hpp"

using std::cout;
using std::endl;
using std::wcout;
using json = nlohmann::json;

int main()
{
	std::fstream f("students.json");
	json jsonData = json::parse(f);
	cout << jsonData;
}
