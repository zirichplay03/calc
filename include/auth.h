#ifndef AUTH_H
#define AUTH_H

#include <string>
#include <sys/socket.h>
#include <sqlite3.h>
#include <iostream>
#include <string>
#include <unistd.h>

#define BUFFER_SIZE 4096

class Auth {
public:
    explicit Auth(std::string dbPath);

    bool authenticate(int clientSocket);

    std::string getInput(int clientSocket, const std::string& prompt);
    std::string dbPath;
    double getBalance(const std::string& username);
    std::string getAuthenticatedUsername();
    bool updateBalance(const std::string& username, double newBalance);
    void logAction(const std::string& action);

private:
    std::string username;
    double balance;
    ssize_t bytesRecv ;
    char buffer[BUFFER_SIZE];
    std::string input;
    std::string password;
    sqlite3* db;
    sqlite3_stmt* stmt;
    std::string sql;
    int rc;
    const unsigned char* storedPassword;
};

#endif  // AUTH_H