cmake_minimum_required(VERSION 3.20)
project(BankSystem VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# На Windows (MinGW) вшиваємо стандартну бібліотеку: bank.exe запускається без DLL MinGW
if(MINGW)
    add_link_options(-static)
endif()

# Логіка без вводу-виводу: її збирають і програма, і тести
add_library(bank_core src/money.cpp src/bank.cpp src/storage.cpp)
target_include_directories(bank_core PUBLIC src)

add_executable(bank src/main.cpp)
target_link_libraries(bank PRIVATE bank_core)

enable_testing()
foreach(name money bank storage)
    add_executable(test_${name} tests/test_${name}.cpp)
    target_link_libraries(test_${name} PRIVATE bank_core)
    add_test(NAME ${name} COMMAND test_${name})
endforeach()
