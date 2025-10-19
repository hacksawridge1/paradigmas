#ifndef DB_H
#define DB_H

#include <string>
#include <string_view>
#include <cctype>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/client.hpp>
#include "convert.h"
#include "console.h"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

bsoncxx::v_noabi::document::value bykw(std::string p_key, std::string p_value)
{
  return make_document(kvp(p_key, p_value));
}

void show_all(mongocxx::collection collection)
{
  auto result = collection.find({});
  for (auto &&doc : result)
  {
    print(bsoncxx::to_json(doc));
  };
}

void show_many(mongocxx::collection collection, std::string p_key, std::string p_value)
{
  auto result = collection.find(bykw(p_key, p_value));
  for (auto &&doc : result)
  {
    print(bsoncxx::to_json(doc));
  };
}

void show_by_first_letter(mongocxx::collection collection, std::string_view p_key, wchar_t p_ch)
{
  auto result = collection.find({});
  for (auto &&doc : result)
  {
    std::string_view value = std::string_view(doc[p_key].get_string());
    wchar_t value_first_letter = asciiToUtf(value)[0];
    if (std::tolower(value_first_letter) == std::tolower(p_ch))
    {
      print(bsoncxx::to_json(doc));
    }
  }
}

#endif