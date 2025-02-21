#include "client.h"

/**
 * @brief Конструктор класса TcpClient.
 * @param ip IP-адрес сервера.
 * @param port Порт для подключения (по умолчанию 5400).
 */
TcpClient::TcpClient(std::string ip, int port)
        : serverIp(std::move(ip)), serverPort(port), sock(-1), sendRes(0), bytesReceived(0), serverAddr{} {

    // Создание сокета
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        throw std::runtime_error("Error: Could not create socket!");
    }

    // Настройка структуры адреса сервера
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr) <= 0) {
        close(sock);
        throw std::runtime_error("Error: Invalid IP address format!");
    }
}

/**
 * @brief Деструктор класса TcpClient.
 * Закрывает сокет при уничтожении объекта.
 */
TcpClient::~TcpClient() {
    if (sock != -1) {
        close(sock);
    }
}

/**
 * @brief Устанавливает соединение с сервером.
 * @return true, если соединение успешно, иначе false.
 */
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

/**
 * @brief Обеспечивает взаимодействие клиента с сервером.
 */
void TcpClient::communicate() {
    std::array<char, BUFFER_SIZE> buf{};

    // Получаем и отображаем меню от сервера
    memset(buf.data(), 0, buf.size());
    bytesReceived = recv(sock, buf.data(), buf.size(), 0);
    if (bytesReceived > 0) {
        std::cout << "SERVER> " << std::string(buf.data(), bytesReceived) << std::endl;
    } else {
        std::cerr << "Error: Failed to receive menu!" << std::endl;
        return;
    }

    // Основной цикл общения с сервером
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, userInput);

        // Завершаем соединение, если пользователь ввел "exit"
        if (userInput == "exit") break;

        // Отправка данных на сервер
        sendRes = send(sock, userInput.c_str(), userInput.size(), 0);
        if (sendRes == -1) {
            std::cerr << "Error: Failed to send data to server!" << std::endl;
            continue;
        }

        // Получаем ответ от сервера
        memset(buf.data(), 0, buf.size());
        bytesReceived = recv(sock, buf.data(), buf.size(), 0);
        if (bytesReceived > 0) {
            std::cout << "SERVER> " << std::string(buf.data(), bytesReceived) << std::endl;
        } else {
            std::cerr << "Error: Failed to receive result!" << std::endl;
        }
    }
}

/**
 * @brief Запускает клиент: устанавливает соединение и инициирует взаимодействие.
 */
void TcpClient::start() {
    if (connectToServer()) {
        communicate();
    }
}

/**
 * @brief Точка входа в программу.
 * @return Код завершения программы (0 - успешное выполнение, 1 - ошибка).
 */
int main() {
    try {
        TcpClient client("127.0.0.1"); // Подключение к локальному серверу
        client.start();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
