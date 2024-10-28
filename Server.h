#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <winsock2.h>

class Server {
public:
    Server(int port);
    ~Server();

    bool initialize();
    bool createSocket();
    bool bindSocket();
    bool ListenSocket();
    bool acceptSocket();

private:
    int port;
    SOCKET serverSocket;
    sockaddr_in serverAddress;

    void cleanup();
};


#endif