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

class DB
{
private:
  mongocxx::instance inst;
  std::string db_name;
  mongocxx::uri uri;
  mongocxx::client client;
  mongocxx::v_noabi::collection collection;
  mongocxx::v_noabi::database db;

  void set_db(std::string p_db_name)
  {
    db = client[db_name];
  }

public:
  DB(std::string p_uri,
     std::string p_db_name,
     std::string p_collection) : uri(p_uri),
                                 db_name(p_db_name),
                                 client(uri)
  {
    set_db(db_name);
    set_collection(p_collection);
  }

  void set_collection(std::string p_collection)
  {
    collection = db[p_collection];
  }

  void show_one(std::string p_key, std::string p_value)
  {
    auto result = collection.find_one(bykw(p_key, p_value));
  }

  bsoncxx::v_noabi::document::value bykw(std::string p_key, std::string p_value)
  {
    return make_document(kvp(p_key, p_value));
  }

  void show_all()
  {
    auto result = collection.find({});
    for (auto &&doc : result)
    {
      Console::print(bsoncxx::to_json(doc));
    };
  }

  void show_many(std::string p_key, std::string p_value)
  {
    auto result = collection.find(bykw(p_key, p_value));
    for (auto &&doc : result)
    {
      Console::print(bsoncxx::to_json(doc));
    };
  }

  void show_by_first_letter(std::string_view p_key, wchar_t p_ch)
  {
    auto result = collection.find({});
    for (auto &&doc : result)
    {
      std::string_view value = std::string_view(doc[p_key].get_string());
      wchar_t value_first_letter = Convert::asciiToUtf(value)[0];
      if (std::tolower(value_first_letter) == std::tolower(p_ch))
      {
        Console::print(bsoncxx::to_json(doc));
      }
    }
  }

  void show_by_first_letter_and_rate_higher_then_4(std::string p_key, wchar_t p_ch)
  {
  }
};

#endif