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
    explicit TcpServer(Auth& authRef);
    ~TcpServer();

    void start_server();

private:
    Auth& auth;
    int serverSocket;
    int opt;
    int clientSocket;
    std::string result_cal;
    struct sockaddr_in serverAdderss;
    ssize_t bytesRecv;
    std::string command;
    double balance;
    double a, b;
    char op;
    std::string username;
    bool running;
    std::string balanceMessage;
    std::vector<std::thread> clientThreads;

    void bindAndlisten();
    void handleClient(int clientSocket);

    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];
    sockaddr_in client;
    socklen_t clientSize;
};

#endif // SERVER_H
