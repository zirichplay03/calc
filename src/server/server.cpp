#include "server.h"
#include <cstring>
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
    serverAdderss.sin_addr.s_addr = INADDR_ANY;}
TcpServer::~TcpServer() {
    // Закрытие всех соединений и очистка ресурсов
    close(serverSocket);
    for (auto& t : clientThreads) {
        if (t.joinable()) {
            t.join();
        }
    }
}
void TcpServer::star_server() {
    bindAndlisten();
    // Ожидание подключения клиентов
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
            std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;        }
        // Создаем поток для обработки клиента
        std::thread clientThread(&TcpServer::handleClient, this, clientSocket);
        clientThreads.push_back(std::move(clientThread));
    }}
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
void TcpServer::handleClient(int clientSocket) {
    char buf[BUFFER_SIZE];
    while (true) {
        memset(buf, 0, BUFFER_SIZE);
        int bytesRecv = recv(clientSocket, buf, BUFFER_SIZE, 0);
        if (bytesRecv == -1) {
            std::cerr << "There was a connection issue" << std::endl;
            break;
        }        if (bytesRecv == 0) {
            std::cout << "The client disconnected" << std::endl;
            break;
        }
        std::cout << "Received: " << std::string(buf, 0, bytesRecv) << std::endl;
        // Отправляем данные обратно клиенту
        send(clientSocket, buf, bytesRecv, 0);
    }
    close(clientSocket);}
void TcpServer::stopServer() {
    // Закрытие сервера
    close(serverSocket);
}

int main(){
TcpServer server;
server.star_server();
EXIT_SUCCESS;
}
