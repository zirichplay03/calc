#ifndef AUTH_H
#define AUTH_H

#include <string>
#include <sys/socket.h>

class Auth {
public:
    // Конструктор, принимающий путь к базе данных
    Auth(const std::string& dbPath);

    // Метод для аутентификации пользователя по логину и паролю
    bool authenticate(int clientSocket);

    std::string getInput(int clientSocket, const std::string& prompt);  // Запрос логина и пароля
    std::string dbPath;  // Путь к базе данных
    double getBalance(const std::string& username);
    std::string getAuthenticatedUsername();
    bool updateBalance(const std::string& username, double newBalance);

private:
    std::string username;  // Имя пользователя
    double balance;
};

#endif  // AUTH_H