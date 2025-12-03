#include "db.h"

class UniversityDB : public DB
{
private:
  mongocxx::collection lessons_collection;

public:
  UniversityDB(std::string p_uri, std::string p_db_name)
      : DB(p_uri, p_db_name, "students")
  {
    set_lessons_collection();
  }

  void set_lessons_collection()
  {
    lessons_collection = db["lessons"];
  }

  void show_students_with_high_ratings(wchar_t first_letter)
  {
    auto students_cursor = collection.find({});

    for (auto &&student : students_cursor)
    {
      process_student(student, first_letter);
    }
  }

private:
  void process_student(const bsoncxx::document::view &student, wchar_t first_letter)
  {
    std::string name = std::string(student["name"].get_string().value);
    std::wstring wide_name = Convert::asciiToUtf(name);

    if (wide_name.empty() || std::tolower(wide_name[0]) != std::tolower(first_letter))
    {
      return;
    }

    std::string surname = std::string(student["surname"].get_string().value);
    std::string group = std::string(student["group"].get_string().value);
    std::string subgroup = std::string(student["subgroup"].get_string().value);

    if (!has_high_rating(name, surname))
    {
      return;
    }

    print_student_info(name, surname, group, subgroup);
    print_student_ratings(name, surname);
  }

  bool has_high_rating(const std::string &name, const std::string &surname)
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

  void print_student_info(const std::string &name, const std::string &surname,
                          const std::string &group, const std::string &subgroup)
  {
    Console::print(L"\n------------------------------");
    Console::print(L"Name: " + Convert::asciiToUtf(name));
    Console::print(L"Surname: " + Convert::asciiToUtf(surname));
    Console::print(L"Group: " + Convert::asciiToUtf(group));
    Console::print(L"Subgroup: " + Convert::asciiToUtf(subgroup));
  }

  void print_student_ratings(const std::string &name, const std::string &surname)
  {
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
          std::wstring output = L"  " + Convert::asciiToUtf(lesson_name) +
                                L": " + std::to_wstring(grade);
          Console::print(output);
        }
      }
    }
    Console::print(L"----------------------------");
  }
};