#include <iostream> 
#include <sys/types.h> 
#include <sys/socket.h>  // Добавлен заголовочный файл для socket(), bind(), listen(), accept() 
#include <netinet/in.h>  // Добавлен заголовочный файл для sockaddr_in 
#include <arpa/inet.h>   // Добавлен заголовочный файл для inet_pton(), inet_ntop() 
#include <unistd.h>      // Добавлен заголовочный файл для close() 
#include <cstring>       // Для memset() 
#include <string> 
#include <netdb.h>
 
using namespace std; 
 
int main() { 
    // Создание сокета 
    int listening = socket(AF_INET, SOCK_STREAM, 0); 
    if (listening == -1) { 
        cerr << "Can't create a socket!" << endl; 
        return -1; 
    } 
 
    // Заполнение структуры sockaddr_in 
    sockaddr_in hint; 
    hint.sin_family = AF_INET; 
    hint.sin_port = htons(5400); 
    hint.sin_addr.s_addr = INADDR_ANY; // Автоматически выбираем доступный IP 
 
    // Привязка сокета 
    if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1) { 
        cerr << "Can't bind to IP/port" << endl; 
        return -2; 
    } 
 
    // Запуск прослушивания 
    if (listen(listening, SOMAXCONN) == -1) { 
        cerr << "Can't listen!" << endl; 
        return -3; 
    } 
 
    // Ожидание подключения клиента 
    sockaddr_in client; 
    socklen_t clientSize = sizeof(client); 
    char host[NI_MAXHOST]; 
    char svc[NI_MAXSERV]; 
 
    int clientSocket = accept(listening, (sockaddr*)&client, &clientSize); 
    if (clientSocket == -1) { 
        cerr << "Problem with client connecting!" << endl; 
        return -4; 
    } 
 
    // Закрываем серверный сокет, так как он больше не нужен 
    close(listening); 
 
    // Очищаем буферы 
    memset(host, 0, NI_MAXHOST); 
    memset(svc, 0, NI_MAXSERV); 
 
    // Получаем информацию о клиенте 
    int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, 
                             svc, NI_MAXSERV, 0); 
 
    if (result == 0) { 
        cout << host << " connected on " << svc << endl; 
    } else { 
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST); 
        cout << host << " connected on " << ntohs(client.sin_port) << endl; 
    } 
 
    // Буфер для приема данных 
    char buf[4096]; 
 
    while (true) { 
        memset(buf, 0, 4096); 
 
        // Получаем данные от клиента 
        int bytesRecv = recv(clientSocket, buf, 4096, 0); 
        if (bytesRecv == -1) { 
            cerr << "There was a connection issue" << endl; 
            break; 
        } 
        if (bytesRecv == 0) { 
            cout << "The client disconnected" << endl; 
            break; 
        } 
 
        cout << "Received: " << string(buf, 0, bytesRecv) << endl; 
 
        // Отправляем данные обратно клиенту 
        send(clientSocket, buf, bytesRecv, 0); 
    } 
 
    // Закрываем клиентский сокет 
    close(clientSocket); 
 
    return 0; 
}
