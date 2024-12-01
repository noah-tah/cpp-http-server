#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include <string>

class Server {
public:
    Server(const std::string& port);
    ~Server();
    void run();

private:
    std::string port;
    SOCKET listenSocket;
    void handleClient(SOCKET clientSocket);
};

#endif