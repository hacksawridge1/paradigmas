#include <string>
#include <iostream>
#include <codecvt>
#include <locale>
#include <variant>
#include <string_view>
#include <cctype>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/client.hpp>

int main()
{
  setlocale(LC_ALL, "");
  mongocxx::instance inst;
  mongocxx::uri uri("mongodb://localhost:27017");
  mongocxx::client client(uri);
  mongocxx::database db = client["university"];
  mongocxx::collection collection = db["students"];
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

  auto result = collection.find({});
  for (auto &&doc : result)
  {
    std::string_view value = std::string_view(doc["name"].get_string());
    std::wstring wideJson = converter.from_bytes(std::string(value));
    wchar_t value_first_letter = wideJson[0];
    if (std::tolower(value_first_letter) == std::tolower(L'\u0410'))
    {
      std::wstring wideData = converter.from_bytes(std::string(bsoncxx::to_json(doc)));
      std::wcout << wideData << std::endl;
    }
  }
  return 0;
}