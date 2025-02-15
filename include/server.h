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

//Порт и буффер
#define SERVER_PORT 5400
#define BUFFER_SIZE 4096

class TcpServer{
public:

   TcpServer();//Конструктор
   ~TcpServer();//Деструктор для закрытие сокета 

   void star_server();

private:

   int serverSocket; //Сокет Сервера
   struct sockaddr_in serverAdderss; //Адрес сервера
   std::vector<std::thread> clientThreads; // Контейнер потоков для обработки клиентов 

   void bindAndlisten(); // Метод для привязки сокета и прослушивания порта 

   void handleClient(int clientSocket); //Метод для обработки подключения клиента 

   void stopServer(); //Метод для остановки сервера 

   char host[NI_MAXHOST];
   char svc[NI_MAXSERV];
   sockaddr_in client;
   socklen_t clientSize;

   
};

#endif //SERVER_H
