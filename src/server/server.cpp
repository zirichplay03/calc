#include "server.h"

TcpServer::TcpServer(Auth& authRef)
        : auth(authRef), clientSocket(-1), bytesRecv(0), balance(0.0), a(0.0), b(0.0), op(0),
          running(true), serverAdderss{}, client{}, host{}, svc{} {

    clientSize = sizeof(client);

    memset(host, 0, NI_MAXHOST);
    memset(svc, 0, NI_MAXSERV);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Can't create a socket!" << std::endl;
        exit(1);
    }

    opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "setsockopt failed!" << std::endl;
        close(serverSocket);
        exit(1);
    }

    serverAdderss.sin_family = AF_INET;
    serverAdderss.sin_port = htons(SERVER_PORT);
    serverAdderss.sin_addr.s_addr = INADDR_ANY;
}

TcpServer::~TcpServer() {
    close(serverSocket);
    for (auto& t : clientThreads) {
        if (t.joinable()) {
            t.join();
        }
    }
}

void TcpServer::bindAndlisten() {
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
    while (running) {
        clientSocket = accept(serverSocket, (sockaddr*)&client, &clientSize);
        if (clientSocket == -1) {
            if (!running)
                break;
            std::cerr << "Problem with client connecting!" << std::endl;
            continue;
        }

        while (!auth.authenticate(clientSocket)) {
            send(clientSocket, "Authentication failed. Try again.\n", 35, 0);
        }

        send(clientSocket, "Authentication successful.\n", 27, 0);
        auth.logAction("User " + auth.getAuthenticatedUsername() + " logged in");
        handleClient(clientSocket); // Передаем `auth`

        close(clientSocket);
    }
}

void TcpServer::handleClient(int socket) {
    char buffer[BUFFER_SIZE];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        bytesRecv = recv(socket, buffer, sizeof(buffer), 0);
        if (bytesRecv == -1) {
            std::cerr << "Error receiving data!" << std::endl;
            break;
        }
        if (bytesRecv == 0) {
            std::cout << "The client disconnected" << std::endl;
            break;
        }

        buffer[bytesRecv] = '\0';
        command = std::string(buffer);

        if (command == "calc") {
            username = auth.getAuthenticatedUsername();
            auth.logAction("User " + username + " tried to use the calculator");

            balance = auth.getBalance(username);
            if (balance <= 0) {
                send(socket, "Insufficient balance to use the calculator.\n", 46, 0);
                auth.logAction("User " + username + " failed to use the calculator due to insufficient balance");
                continue;
            }

            send(socket, "Welcome to the calculator! Please choose an operation:\n1. Addition (+)\n2. Subtraction (-)\n3. Multiplication (*)\n4. Division (/)\n", 130, 0);

            memset(buffer, 0, sizeof(buffer));
            bytesRecv = recv(socket, buffer, sizeof(buffer), 0);
            if (bytesRecv == -1 || bytesRecv == 0) {
                std::cerr << "Error receiving calculation expression!" << std::endl;
                break;
            }

            buffer[bytesRecv] = '\0';
            if (sscanf(buffer, "%lf %c %lf", &a, &op, &b) == 3) {
                result_cal = calculate(a, b, op);
                send(socket, result_cal.c_str(), result_cal.size(), 0);

                balance -= 1;
                auth.updateBalance(username, balance);
            } else {
                send(socket, "Invalid expression format. Try again.\n", 39, 0);
            }

        } else if (command == "balance") {
            username = auth.getAuthenticatedUsername();
            auth.logAction("User " + username + " checked balance");

            balance = auth.getBalance(username);
            balanceMessage = "Your balance is: " + std::to_string(balance) + "\n";
            send(socket, balanceMessage.c_str(), balanceMessage.size(), 0);

        } else {
            send(socket, "Invalid command. Try again.\n", 26, 0);
        }
    }

    close(socket);
}

int main() {
    Auth auth(DB_PATH);
    TcpServer server(auth);
    server.start_server();
    return EXIT_SUCCESS;
}
