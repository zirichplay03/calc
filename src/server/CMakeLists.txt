set(SERVER_SOURCES
        auth.cpp
        server.cpp
        calculator.cpp
)

# Создание цели для сервера
add_executable(calc_server ${SERVER_SOURCES})

# Добавление зависимостей
target_link_libraries(calc_server pthread)

# Путь к SQL-скрипту
set(SQL_SCRIPT "${CMAKE_SOURCE_DIR}/db/schema.sql")

# Путь к создаваемой базе данных
set(DB_FILE "${CMAKE_BINARY_DIR}/calculator.db")

# Проверка наличия утилиты sqlite3
find_program(SQLITE3_EXECUTABLE sqlite3)

if(NOT SQLITE3_EXECUTABLE)
    message(FATAL_ERROR "sqlite3 is not installed on your system!")
endif()

# Выполнение SQL-скрипта после сборки сервера
add_custom_command(
        TARGET calc_server POST_BUILD
        COMMAND ${SQLITE3_EXECUTABLE} ${DB_FILE} < ${SQL_SCRIPT}
        COMMENT "Creating database and tables..."
)

# Передаем путь к базе данных как макрос в исходный код
add_definitions(-DDB_PATH="${DB_FILE}")