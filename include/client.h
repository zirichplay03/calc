#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <iostream>
#include <string>
#include <cstring>
#include <array>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "calculator.h"

#define DEFAULT_PORT 5400         
#define BUFFER_SIZE 4096          

class TcpClient {
public:
    explicit TcpClient(std::string ip, int port = DEFAULT_PORT);
    ~TcpClient();
    void start();

private:
    int sock;
    std::string serverIp;
    int serverPort;
    sockaddr_in serverAddr;
    std::string userInput;
    ssize_t sendRes;
    ssize_t bytesReceived;

    bool connectToServer();
    void communicate();
};

#endif // TCP_CLIENT_H
