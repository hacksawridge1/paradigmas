#include "lib/db.h"
#include <windows.h>

int main()
{
	SetConsoleOutputCP(CP_UTF8);
	setlocale(LC_ALL, "");
	std::wcout.imbue(std::locale(""));
	DB db("mongodb://localhost:27017", "university", "students");
	db.show_by_first_letter("name", L'\u0410');
	return 0;
}