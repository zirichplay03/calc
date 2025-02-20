#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <netdb.h>
#include "calculator.h"
#include "auth.h"
#include <cstring>

// Порт и буфер
#define SERVER_PORT 5400
#define BUFFER_SIZE 4096

class TcpServer {
public:
    TcpServer();   // Конструктор
    ~TcpServer();  // Деструктор для закрытия сокета

    void start_server();

private:
    int serverSocket; // Сокет сервера
    int opt;
    int clientSocket;
    std::string result_cal;
    struct sockaddr_in serverAdderss; // Адрес сервера
    ssize_t bytesRecv;
    std::string command;
    double balance;
    double a, b;
    char op;
    std::string username;
    bool running;
    std::string balanceMessage;
    std::vector<std::thread> clientThreads; // Контейнер потоков для обработки клиентов

    void bindAndlisten();// Метод для привязки сокета и прослушивания порта
    void handleClient(int clientSocket, Auth& auth); // Метод для обработки подключения клиента
    void stopServer();// Метод для остановки сервера

    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];
    sockaddr_in client;
    socklen_t clientSize;
};

#endif // SERVER_H
