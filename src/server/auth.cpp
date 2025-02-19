#include "auth.h"
#include <sqlite3.h>
#include <iostream>
#include <string>
#include <unistd.h>

Auth::Auth(const std::string& dbPath) : dbPath(dbPath) {}

std::string Auth::getInput(int clientSocket, const std::string& prompt) {
    char buffer[1024];
    send(clientSocket, prompt.c_str(), prompt.size(), 0);

    int bytesRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRecv <= 0) {
        std::cerr << "Error receiving input!" << std::endl;
        close(clientSocket);
        return "";
    }

    return std::string(buffer);
}

bool Auth::authenticate(int clientSocket) {
    // Запрос логина
    std::string username = getInput(clientSocket, "Enter username: ");
    if (username.empty()) return false; // Если пустое имя, выходим

    // Запрос пароля
    std::string password = getInput(clientSocket, "Enter password: ");
    if (password.empty()) return false; // Если пустой пароль, выходим

    // Проверка учетных данных в базе
    sqlite3* db;
    sqlite3_stmt* stmt;
    std::string sql = "SELECT password FROM users WHERE username = ?;";  // SQL-запрос для поиска пароля пользователя

    // Открытие базы данных
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Подготовка SQL-запроса
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }

    // Привязка параметра (имя пользователя)
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    // Выполнение запроса
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        // Получение хешированного пароля из базы данных
        const unsigned char* storedPassword = sqlite3_column_text(stmt, 0);

        // Для безопасности следует использовать bcrypt для проверки пароля,
        // но для упрощения здесь просто сравниваем строку (это не безопасно в реальных приложениях).
        if (storedPassword && password == reinterpret_cast<const char*>(storedPassword)) {
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return true;  // Аутентификация успешна
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return false;  // Неверные данные
}
double Auth::getBalance(const std::string& username) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    std::string sql = "SELECT balance FROM users WHERE username = ?;";  // SQL-запрос для получения баланса пользователя

    // Открытие базы данных
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return -1.0;  // Ошибка при открытии базы данных
    }

    // Подготовка SQL-запроса
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return -1.0;  // Ошибка при подготовке запроса
    }

    // Привязка параметра (имя пользователя)
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    // Выполнение запроса
    rc = sqlite3_step(stmt);
    double balance = -1.0;  // Значение по умолчанию для ошибки
    if (rc == SQLITE_ROW) {
        // Получаем баланс из базы данных
        balance = sqlite3_column_double(stmt, 0);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return balance;
}