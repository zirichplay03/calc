#include "auth.h"

Auth::Auth(const std::string& dbPath) : dbPath(dbPath) {}

std::string Auth::getInput(int clientSocket, const std::string& prompt) {
    send(clientSocket, prompt.c_str(), prompt.size(), 0);

    bytesRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRecv <= 0) {
        std::cerr << "Error receiving input!" << std::endl;
        close(clientSocket);
        return "";
    }

    buffer[bytesRecv] = '\0';  // Завершаем строку
    input=std::string (buffer);

    // Убираем символ новой строки, если он есть
    if (!input.empty() && input[input.size() - 1] == '\n') {
        input.erase(input.size() - 1);  // Удаляем последний символ новой строки
    }

    return input;
}

bool Auth::authenticate(int clientSocket) {
    // Запрос логина
    username = getInput(clientSocket, "Enter username: ");
    if (username.empty()) return false; // Если пустое имя, выходим

    // Запрос пароля
    password = getInput(clientSocket, "Enter password: ");
    if (password.empty()) return false; // Если пустой пароль, выходим

    sql = "SELECT password FROM users WHERE username = ?;";  // SQL-запрос для поиска пароля пользователя

    // Открытие базы данных
    rc = sqlite3_open(dbPath.c_str(), &db);
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
        // Получение пароля из базы данных
        storedPassword = sqlite3_column_text(stmt, 0);

        // Логируем полученный пароль
        std::cout << "Stored password: " << storedPassword << std::endl;
        std::cout << "Entered password: " << password << std::endl;

        if (storedPassword && password == reinterpret_cast<const char*>(storedPassword)) {
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return true;  // Аутентификация успешна
        } else {
            std::cerr << "Password mismatch." << std::endl;
        }
    } else {
        std::cerr << "User not found." << std::endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return false;  // Неверные данные
}

std::string Auth::getAuthenticatedUsername() {
    return username;  // Возвращаем имя пользователя
}
bool Auth::updateBalance(const std::string& username, double newBalance) {
    sql = "UPDATE users SET balance = ? WHERE username = ?;";  // SQL-запрос для обновления баланса пользователя

    // Открытие базы данных
    rc = sqlite3_open(dbPath.c_str(), &db);
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

    // Привязка параметров (новый баланс и имя пользователя)
    sqlite3_bind_double(stmt, 1, newBalance);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);

    // Выполнение запроса
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to update balance: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return false;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return true;  // Баланс успешно обновлен
}
double Auth::getBalance(const std::string& username) {
    sql = "SELECT balance FROM users WHERE username = ?;";  // SQL-запрос для получения баланса пользователя

    // Открытие базы данных
    rc = sqlite3_open(dbPath.c_str(), &db);
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
    balance = -1.0;  // Значение по умолчанию для ошибки
    if (rc == SQLITE_ROW) {
        // Получаем баланс из базы данных
        balance = sqlite3_column_double(stmt, 0);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return balance;
}

// В файле auth.cpp добавьте:
void Auth::logAction(const std::string& action) {
    sql = "INSERT INTO logs (username, action) VALUES (?, ?);";  // SQL-запрос для записи в журнал

    // Открытие базы данных
    rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    // Подготовка SQL-запроса
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return;
    }

    // Привязка параметров (имя пользователя и действие)
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, action.c_str(), -1, SQLITE_STATIC);

    // Выполнение запроса
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to insert log: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}