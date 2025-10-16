#include <string>
#include <fstream>
#include "json.hpp"

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

	std::string getJson(int indent = 2)
	{
		return content.dump(indent);
	}
};