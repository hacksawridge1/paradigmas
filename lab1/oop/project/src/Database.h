#ifndef DATABASE_H
#define DATABASE_H

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <iostream>
#include <string>

using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_document;

class Database
{
private:
  mongocxx::instance instance{};
  mongocxx::client client;
  mongocxx::database db;
  mongocxx::collection accounts;

public:
  Database(const std::string &uri, const std::string &db_name, const std::string &collection_name);

  bool createAccount(const std::string &account_id, const std::string &name, double initial_balance);
  bool deposit(const std::string &account_id, double amount);
  bool withdraw(const std::string &account_id, double amount);
  double getBalance(const std::string &account_id);
  bool accountExists(const std::string &account_id);
  void displayAccountInfo(const std::string &account_id);
  bool deleteAccount(const std::string &account_id);
};

#endif