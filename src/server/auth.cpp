#include "auth.h"

/**
 * @brief Конструктор класса Auth.
 * @param dbPath Путь к базе данных SQLite.
 */
Auth::Auth(std::string dbPath)
        : dbPath(std::move(dbPath)), balance(0.0), bytesRecv(0), buffer{}, db(nullptr), stmt(nullptr), rc(0), storedPassword(nullptr) {}

/**
 * @brief Получает ввод пользователя через сокет.
 * @param clientSocket Сокет клиента.
 * @param prompt Сообщение для пользователя перед вводом.
 * @return Строка, содержащая ввод пользователя.
 */
std::string Auth::getInput(int clientSocket, const std::string& prompt) {
    send(clientSocket, prompt.c_str(), prompt.size(), 0);

    bytesRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRecv <= 0) {
        std::cerr << "Error receiving input!" << std::endl;
        close(clientSocket);
        return "";
    }

    buffer[bytesRecv] = '\0';  // Завершаем строку
    input = std::string(buffer);

    // Убираем символ новой строки, если он есть
    if (!input.empty() && input[input.size() - 1] == '\n') {
        input.erase(input.size() - 1);
    }
    return input;
}

/**
 * @brief Выполняет аутентификацию пользователя.
 * @param clientSocket Сокет клиента.
 * @return true, если аутентификация успешна, иначе false.
 */
bool Auth::authenticate(int clientSocket) {
    username = getInput(clientSocket, "Enter username: ");
    if (username.empty()) return false;

    password = getInput(clientSocket, "Enter password: ");
    if (password.empty()) return false;

    sql = "SELECT password FROM users WHERE username = ?;";

    rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        storedPassword = sqlite3_column_text(stmt, 0);

        std::cout << "Stored password: " << storedPassword << std::endl;
        std::cout << "Entered password: " << password << std::endl;

        if (storedPassword && password == reinterpret_cast<const char*>(storedPassword)) {
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return true;
        } else {
            std::cerr << "Password mismatch." << std::endl;
        }
    } else {
        std::cerr << "User not found." << std::endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return false;
}

/**
 * @brief Получает аутентифицированное имя пользователя.
 * @return Имя пользователя.
 */
std::string Auth::getAuthenticatedUsername() {
    return username;
}

/**
 * @brief Обновляет баланс пользователя в базе данных.
 * @param username Имя пользователя.
 * @param newBalance Новый баланс.
 * @return true, если обновление прошло успешно, иначе false.
 */
bool Auth::updateBalance(const std::string& username, double newBalance) {
    sql = "UPDATE users SET balance = ? WHERE username = ?;";

    rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }

    sqlite3_bind_double(stmt, 1, newBalance);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to update balance: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return false;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return true;
}

/**
 * @brief Получает текущий баланс пользователя.
 * @param username Имя пользователя.
 * @return Баланс пользователя. В случае ошибки возвращает -1.0.
 */
double Auth::getBalance(const std::string& username) {
    sql = "SELECT balance FROM users WHERE username = ?;";

    rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return -1.0;
    }

    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return -1.0;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    balance = -1.0;
    if (rc == SQLITE_ROW) {
        balance = sqlite3_column_double(stmt, 0);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return balance;
}

/**
 * @brief Записывает действие пользователя в журнал.
 * @param action Описание действия.
 */
void Auth::logAction(const std::string& action) {
    sql = "INSERT INTO logs (username, action) VALUES (?, ?);";

    rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, action.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to insert log: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
