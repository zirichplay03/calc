cmake_minimum_required(VERSION 3.26)
project(calc)

set(CMAKE_CXX_STANDARD 11)

# Указание включаемых директорий
include_directories(${CMAKE_SOURCE_DIR}/include)

# Подкаталоги для клиента и сервера
add_subdirectory(src/client)
add_subdirectory(src/server)

# Найдем SQLite3
find_package(SQLite3 REQUIRED)

# Добавим SQLite3 в список зависимостей
target_link_libraries(calc_server SQLite::SQLite3 pthread)