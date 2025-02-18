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
};

#endif  // AUTH_H