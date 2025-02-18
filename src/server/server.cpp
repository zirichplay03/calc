#include "auth.h"
#include "server.h"
#include <cstring>
#include <iostream>

TcpServer::TcpServer() {
    // Инициализация переменных
    clientSize = sizeof(client);
    memset(host, 0, NI_MAXHOST);
    memset(svc, 0, NI_MAXSERV);
    // Создание сокета
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Can't create a socket!" << std::endl;
        exit(1);
    }
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "setsockopt failed!" << std::endl;
        close(serverSocket);
        exit(1);
    }
    // Настройка адреса сервера
    serverAdderss.sin_family = AF_INET;
    serverAdderss.sin_port = htons(SERVER_PORT);
    serverAdderss.sin_addr.s_addr = INADDR_ANY;
}

TcpServer::~TcpServer() {
    // Закрытие всех соединений и очистка ресурсов
    close(serverSocket);
    for (auto& t : clientThreads) {
        if (t.joinable()) {
            t.join();
        }
    }
}

void TcpServer::bindAndlisten() {
    // Привязка сокета и прослушивание порта
    if (bind(serverSocket, (sockaddr*)&serverAdderss, sizeof(serverAdderss)) == -1) {
        std::cerr << "Can't bind to IP/port" << std::endl;
        exit(1);
    }
    if (listen(serverSocket, SOMAXCONN) == -1) {
        std::cerr << "Can't listen!" << std::endl;
        exit(1);
    }
}

void TcpServer::start_server() {
    bindAndlisten();
    while (true) {
        int clientSocket = accept(serverSocket, (sockaddr*)&client, &clientSize);
        if (clientSocket == -1) {
            std::cerr << "Problem with client connecting!" << std::endl;
            continue;
        }

        // Получаем информацию о клиенте
        int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
        if (result == 0) {
            std::cout << host << " connected on " << svc << std::endl;
        } else {
            inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
            std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
        }

        // Передаем путь к базе данных через макрос DB_PATH
        Auth auth(DB_PATH);  // Создаем объект Auth для работы с базой данных

        // Выполняем аутентификацию с логином и паролем
        if (auth.authenticate(clientSocket)) {
            send(clientSocket, "Authentication successful.\n", 27, 0);

            // Логируем успешную аутентификацию
            std::string username = auth.getAuthenticatedUsername();
            auth.logAction("User " + username + " logged in");

            // После успешной аутентификации переходим к обработке команд
            handleClient(clientSocket, auth);  // Передаем объект auth в handleClient для использования getBalance
        } else {
            send(clientSocket, "Authentication failed.\n", 24, 0);
            close(clientSocket);  // Закрытие соединения при неудачной аутентификации
        }
    }
}

void TcpServer::handleClient(int clientSocket, Auth& auth) {
    char buffer[1024];

    while (true) {
        // Получаем команду от клиента
        memset(buffer, 0, sizeof(buffer));
        int bytesRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRecv == -1) {
            std::cerr << "Error receiving data!" << std::endl;
            break;
        }
        if (bytesRecv == 0) {
            std::cout << "The client disconnected" << std::endl;
            break;
        }

        buffer[bytesRecv] = '\0';  // Завершаем строку
        std::string command(buffer); // Преобразуем команду в строку

        if (command == "calc") {
            // Логируем использование калькулятора
            std::string username = auth.getAuthenticatedUsername();
            auth.logAction("User " + username + " tried to use the calculator");

            // Проверяем баланс пользователя
            double balance = auth.getBalance(username);  // Получаем текущий баланс

            if (balance <= 0) {
                send(clientSocket, "Insufficient balance to use the calculator.\n", 46, 0);
                auth.logAction("User " + username + " failed to use the calculator due to insufficient balance");
                continue;  // Пропускаем команду калькулятора
            }

            send(clientSocket, "Welcome to the calculator! Please choose an operation:\n1. Addition (+)\n2. Subtraction (-)\n3. Multiplication (*)\n4. Division (/)\n", 130, 0);

            // Получаем выражение для калькулятора
            memset(buffer, 0, sizeof(buffer));
            bytesRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesRecv == -1 || bytesRecv == 0) {
                std::cerr << "Error receiving calculation expression!" << std::endl;
                break;
            }

            buffer[bytesRecv] = '\0';  // Завершаем строку
            double a, b;
            char op;
            if (sscanf(buffer, "%lf %c %lf", &a, &op, &b) == 3) {
                std::string result = calculate(a, b, op);  // Вычисляем результат
                send(clientSocket, result.c_str(), result.size(), 0);  // Отправляем результат клиенту

                // Уменьшаем баланс на 1
                balance -= 1;
                auth.updateBalance(username, balance);  // Обновляем баланс в базе данных
            } else {
                send(clientSocket, "Invalid expression format. Try again.\n", 39, 0); // Ошибка при парсинге выражения
            }

        } else if (command == "balance") {
            // Логируем проверку баланса
            std::string username = auth.getAuthenticatedUsername();
            auth.logAction("User " + username + " checked balance");

            // Получаем баланс пользователя
            double balance = auth.getBalance(username);  // Получаем баланс
            std::string balanceMessage = "Your balance is: " + std::to_string(balance) + "\n";
            send(clientSocket, balanceMessage.c_str(), balanceMessage.size(), 0);

        } else if (command == "exit") {
            send(clientSocket, "Goodbye!\n", 9, 0);
            break;  // Закрытие соединения при вводе "exit"

        } else {
            send(clientSocket, "Invalid command. Try again.\n", 26, 0);
        }
    }

    close(clientSocket);  // Закрытие соединения
}

void TcpServer::stopServer() {
    // Закрытие сервера
    close(serverSocket);
}

int main() {
    TcpServer server;
    server.start_server();
    return EXIT_SUCCESS;
}