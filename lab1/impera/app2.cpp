#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/json.hpp>

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

  // Получаем всех студентов с именами на букву 'А'
  auto students_cursor = students_collection.find({});

  for (auto &&student : students_cursor)
  {
    // Преобразуем string_view в string
    std::string_view name_view = student["name"].get_string().value;
    std::string name(name_view);
    std::wstring wide_name = converter.from_bytes(name);

    // Проверяем первую букву имени
    if (wide_name.empty() || std::tolower(wide_name[0]) != std::tolower(L'\u0410'))
    {
      continue;
    }

    // Получаем данные студента
    std::string_view surname_view = student["surname"].get_string().value;
    std::string surname(surname_view);
    std::string group = std::string(student["group"].get_string().value);
    std::string subgroup = std::string(student["subgroup"].get_string().value);

    // Проверяем оценки студента
    auto lessons_cursor = lessons_collection.find({});
    bool found_high_ratings = false;

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

        // Проверяем что это тот же студент и оценка > 4
        if (rating_name == name && rating_surname == surname && grade >= 4)
        {
          if (!found_high_ratings)
          {
            // Выводим заголовок для студента только один раз
            std::wcout << L"\n------------------------------" << std::endl;
            std::wcout << L"Name: " << converter.from_bytes(name) << std::endl;
            std::wcout << L"Surname: " << converter.from_bytes(surname) << std::endl;
            std::wcout << L"Group: " << converter.from_bytes(group) << std::endl;
            std::wcout << L"Subgroup: " << converter.from_bytes(subgroup) << std::endl;
            found_high_ratings = true;
          }

          // Выводим информацию о предмете и оценке
          std::wcout << L"\tLesson: " << converter.from_bytes(lesson_name);
          std::wcout << L" | Rate: " << grade << std::endl;
        }
      }
    }

    if (found_high_ratings)
    {
      std::wcout << L"----------------------------" << std::endl;
    }
  }

  return 0;
}