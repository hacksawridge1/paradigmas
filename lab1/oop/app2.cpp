#include <windows.h>
#include <lib/university.h>

int main()
{
  SetConsoleOutputCP(CP_UTF8);
  setlocale(LC_ALL, "");
  std::wcout.imbue(std::locale(""));

  UniversityDB university_db("mongodb://localhost:27017", "university");
  university_db.show_students_with_high_ratings(L'\u0410');

  return 0;
}