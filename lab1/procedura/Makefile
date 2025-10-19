# Компилятор MSVC
CXX = cl

# Общие флаги компиляции
CXXFLAGS = /std:c++20 /O2 /EHsc -DMONGOCXX_STATIC -DBSONCXX_STATIC

INCLUDES = /I"C:\\mongo-cxx-driver\\include\\bsoncxx\\v_noabi" \
           /I"C:\\mongo-cxx-driver\\include\\mongocxx\\v_noabi" \
					 /I"C:\\mongo-cxx-driver\\include" \
/I"C:\\mongo-cxx-driver\\include\\bson-2.1.2" \
/I"C:\\mongo-cxx-driver\\include\\mongoc-2.1.2"

# Путь к библиотекам
LIB_PATHS = /LIBPATH:"C:\mongo-cxx-driver\lib"

# Выбор библиотек в зависимости от конфигурации (Release или Debug)
!IF "$(CMAKE_BUILD_TYPE)" == "Debug"
LIBS = mongocxx-v_noabi-dhs-x64-v143-mdd.lib bsoncxx-v_noabi-dhs-x64-v143-mdd.lib mongoc2.lib bson2.lib
!ELSE
LIBS = mongocxx-v_noabi-rhs-x64-v143-md.lib bsoncxx-v_noabi-rhs-x64-v143-md.lib mongoc2.lib bson2.lib
!ENDIF

# Имя целевого исполняемого файла
TARGET = app1.exe

# Исходные файлы
SOURCES = *.cpp

# Цель по умолчанию
all: $(TARGET)

# Правило для сборки исполняемого файла
$(TARGET):
	$(CXX) $(CXXFLAGS) $(SOURCES) $(INCLUDES) /link $(LIB_PATHS)  /OUT:$(TARGET) $(LIBS)

# Цель для очистки
clean:
	del /Q $(TARGET) *.obj