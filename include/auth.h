#ifndef AUTH_H
#define AUTH_H

#include <string>
#include <sys/socket.h>
#include <sqlite3.h>
#include <iostream>
#include <string>
#include <unistd.h>

#define BUFFER_SIZE 4096

class Auth {
public:
    // Конструктор, принимающий путь к базе данных
    explicit Auth(std::string dbPath);

    // Метод для аутентификации пользователя по логину и паролю
    bool authenticate(int clientSocket);

    std::string getInput(int clientSocket, const std::string& prompt);  // Запрос логина и пароля
    std::string dbPath;  // Путь к базе данных
    double getBalance(const std::string& username);
    std::string getAuthenticatedUsername();
    bool updateBalance(const std::string& username, double newBalance);
    void logAction(const std::string& action);  // Метод для логирования действий пользователя

private:
    std::string username;  // Имя пользователя
    double balance;
    ssize_t bytesRecv ;
    char buffer[BUFFER_SIZE];
    std::string input;
    std::string password;
    sqlite3* db;
    sqlite3_stmt* stmt;
    std::string sql;
    int rc;
    const unsigned char* storedPassword;
};

#endif  // AUTH_H