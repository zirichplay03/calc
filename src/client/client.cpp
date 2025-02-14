#include "client.h"

TcpClient::TcpClient(const std::string& ip, int port) : serverIp(ip), serverPort(port) {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        throw std::runtime_error("Error: Could not create socket!");
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr) <= 0) {
        close(sock);
        throw std::runtime_error("Error: Invalid IP address format!");
    }
}

TcpClient::~TcpClient() {
    if (sock != -1) {
        close(sock);
    }
}

bool TcpClient::connectToServer() {
    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error: Could not connect to server!" << std::endl;
        close(sock);
        sock = -1;
        return false;
    }
    std::cout << "Connected to " << serverIp << " on port " << serverPort << std::endl;
    return true;
}

void TcpClient::communicate() {
    std::array<char, BUFFER_SIZE> buf{};
    std::string userInput;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, userInput);

        if (userInput == "exit") break;

        int sendRes = send(sock, userInput.c_str(), userInput.size(), 0);
        if (sendRes == -1) {
            std::cerr << "Error: Failed to send data to server!" << std::endl;
            continue;
        }

        memset(buf.data(), 0, buf.size());
        int bytesReceived = recv(sock, buf.data(), buf.size(), 0);
        if (bytesReceived > 0) {
            std::cout << "SERVER> " << std::string(buf.data(), bytesReceived) << std::endl;
        } else {
            std::cerr << "Error: Failed to receive response!" << std::endl;
        }
    }
}

void TcpClient::start() {
    if (connectToServer()) {
        communicate();
    }
}
int main() {
    try {
        TcpClient client("127.0.0.1"); 
        client.start();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
