#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/client.hpp>
#include "lib/db.h"

int main()
{
  setlocale(LC_ALL, "");
  mongocxx::instance inst;
  mongocxx::uri uri("mongodb://localhost:27017");
  mongocxx::client client(uri);
  mongocxx::database db = client["university"];
  mongocxx::collection collection = db["students"];
  show_by_first_letter(collection, "name", L'\u0410');
  return 0;
}