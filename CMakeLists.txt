cmake_minimum_required(VERSION 3.20)
project(BankSystem VERSION 0.2.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Попередження — лише для нашого коду; SQLite збирається як є
if(MSVC)
    set(BANK_WARNINGS /W4 /utf-8)
else()
    set(BANK_WARNINGS -Wall -Wextra -Wpedantic)
endif()

# На Windows (MinGW) вшиваємо стандартну бібліотеку: bank.exe запускається без DLL MinGW
if(MINGW)
    add_link_options(-static)
endif()

# SQLite 3.50.4 — вихідний код у репозиторії (third_party/sqlite), нічого встановлювати не треба (ADR-0004)
add_library(sqlite3 STATIC third_party/sqlite/sqlite3.c)
target_include_directories(sqlite3 PUBLIC third_party/sqlite)
# Програма однопотокова й не завантажує розширення — тому не потрібні pthread і dl
target_compile_definitions(sqlite3 PRIVATE SQLITE_THREADSAFE=0 SQLITE_OMIT_LOAD_EXTENSION SQLITE_DQS=0)
if(UNIX)
    target_link_libraries(sqlite3 PUBLIC m)
endif()

# Логіка без вводу-виводу: її збирають і програма, і тести
add_library(bank_core src/money.cpp src/bank.cpp src/storage.cpp)
target_include_directories(bank_core PUBLIC src)
target_compile_options(bank_core PRIVATE ${BANK_WARNINGS})
target_link_libraries(bank_core PRIVATE sqlite3)

add_executable(bank src/main.cpp)
target_compile_options(bank PRIVATE ${BANK_WARNINGS})
target_link_libraries(bank PRIVATE bank_core)

enable_testing()
foreach(name money bank storage)
    add_executable(test_${name} tests/test_${name}.cpp)
    target_compile_options(test_${name} PRIVATE ${BANK_WARNINGS})
    target_link_libraries(test_${name} PRIVATE bank_core sqlite3)
    add_test(NAME ${name} COMMAND test_${name})
endforeach()
