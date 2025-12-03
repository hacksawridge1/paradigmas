#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/json.hpp>
#include "lib/db.h"
#include "lib/convert.h"
#include "lib/console.h"

bool startsWithA(const std::wstring &name);
bool hasHighRating(mongocxx::collection &lessons_collection,
                   const std::string &name,
                   const std::string &surname);
void printStudentInfo(std::wstring_convert<std::codecvt_utf8<wchar_t>> &converter,
                      const std::string &name,
                      const std::string &surname,
                      const std::string &group,
                      const std::string &subgroup);
void printLessonInfo(std::wstring_convert<std::codecvt_utf8<wchar_t>> &converter,
                     const std::string &lesson_name,
                     int grade);
void processStudent(mongocxx::collection &lessons_collection,
                    std::wstring_convert<std::codecvt_utf8<wchar_t>> &converter,
                    const bsoncxx::document::view &student);

int main()
{
  setlocale(LC_ALL, "");
  mongocxx::instance inst;
  mongocxx::uri uri("mongodb://localhost:27017");
  mongocxx::client client(uri);
  mongocxx::database db = client["university"];
  mongocxx::collection students_collection = db["students"];
  mongocxx::collection lessons_collection = db["lessons"];
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

  auto students_cursor = students_collection.find({});

  for (auto &&student : students_cursor)
  {
    processStudent(lessons_collection, converter, student);
  }

  return 0;
}

bool startsWithA(const std::wstring &name)
{
  return !name.empty() && std::tolower(name[0]) == std::tolower(L'\u0410');
}

bool hasHighRating(mongocxx::collection &lessons_collection,
                   const std::string &name,
                   const std::string &surname)
{
  auto lessons_cursor = lessons_collection.find({});

  for (auto &&lesson : lessons_cursor)
  {
    auto ratings = lesson["rating"].get_array().value;

    for (auto &&rating : ratings)
    {
      auto rating_doc = rating;
      std::string rating_name = std::string(rating_doc["name"].get_string().value);
      std::string rating_surname = std::string(rating_doc["surname"].get_string().value);
      int grade = rating_doc["rating"].get_int32().value;

      if (rating_name == name && rating_surname == surname && grade >= 4)
      {
        return true;
      }
    }
  }
  return false;
}

void printStudentInfo(std::wstring_convert<std::codecvt_utf8<wchar_t>> &converter,
                      const std::string &name,
                      const std::string &surname,
                      const std::string &group,
                      const std::string &subgroup)
{
  print(L"\n------------------------------");
  print(L"Name: " + converter.from_bytes(name));
  print(L"Surname: " + converter.from_bytes(surname));
  print(L"Group: " + converter.from_bytes(group));
  print(L"Subgroup: " + converter.from_bytes(subgroup));
}

void printLessonInfo(std::wstring_convert<std::codecvt_utf8<wchar_t>> &converter,
                     const std::string &lesson_name,
                     int grade)
{
  std::wstring output = L"\tLesson: " + converter.from_bytes(lesson_name) + L" | Rate: " + std::to_wstring(grade);
  print(output);
}

void processStudent(mongocxx::collection &lessons_collection,
                    std::wstring_convert<std::codecvt_utf8<wchar_t>> &converter,
                    const bsoncxx::document::view &student)
{
  std::string name = std::string(student["name"].get_string().value);
  std::wstring wide_name = converter.from_bytes(name);

  std::string surname = std::string(student["surname"].get_string().value);
  std::string group = std::string(student["group"].get_string().value);
  std::string subgroup = std::string(student["subgroup"].get_string().value);

  if (!startsWithA(wide_name))
  {
    return;
  }

  if (!hasHighRating(lessons_collection, name, surname))
  {
    return;
  }

  printStudentInfo(converter, name, surname, group, subgroup);

  auto lessons_cursor = lessons_collection.find({});
  for (auto &&lesson : lessons_cursor)
  {
    std::string lesson_name = std::string(lesson["lesson"].get_string().value);
    auto ratings = lesson["rating"].get_array().value;

    for (auto &&rating : ratings)
    {
      auto rating_doc = rating;
      std::string rating_name = std::string(rating_doc["name"].get_string().value);
      std::string rating_surname = std::string(rating_doc["surname"].get_string().value);
      int grade = rating_doc["rating"].get_int32().value;

      if (rating_name == name && rating_surname == surname && grade >= 4)
      {
        printLessonInfo(converter, lesson_name, grade);
      }
    }
  }

  print(L"----------------------------");
}