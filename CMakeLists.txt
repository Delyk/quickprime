cmake_minimum_required(VERSION 3.10)
project(quickprime)

set(CMAKE_CXX_STANDARD 17)

# Собираем исполняемый файл из всех cpp-файлов в src
add_executable(quickprime
    src/main.cpp
    src/prime.cpp
)

# Устанавливаем путь вывода бинарника в корень проекта
set_target_properties(quickprime PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

