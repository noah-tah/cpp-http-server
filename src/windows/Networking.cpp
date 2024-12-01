#include "..\..\include\Networking.h"
#include <iostream>

Networking::Networking() {
    initializeWinsock();
}

Networking::~Networking() {
    cleanup();
}

void Networking::initializeWinsock() {
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed with error: " << iResult << std::endl;
        exit(1);
    }
}

void Networking::cleanup() {
    WSACleanup();
}


SOCKET Networking::createListenSocket(const std::string& port) {
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // Socket for TCP
    hints.ai_protocol = IPPROTO_TCP; // TCP protocol
    hints.ai_flags = AI_PASSIVE; 

    int iResult = getaddrinfo(NULL, port.c_str(), &hints, &result);
    if (iResult != 0) {
        std::cerr << "getaddrinfo failed: " << iResult << std::endl;
        WSACleanup();
        exit(1);
    }

    SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result); // pAddrInfo 
        WSACleanup();
        exit(1);
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        exit(1);
    }

    std::cout << "Server is listening on port " << port << std::endl;
    return ListenSocket;

}

SOCKET Networking::acceptClient(SOCKET listenSocket, std::string& clientIP) {
    struct sockaddr_in clientInfo;
    int clientInfoSize = sizeof(clientInfo);
    SOCKET ClientSocket = accept(listenSocket, (struct sockaddr*)&clientInfo, &clientInfoSize);
    if (ClientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
        return INVALID_SOCKET;
    }

    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientInfo.sin_addr), ipstr, INET_ADDRSTRLEN);
    clientIP = std::string(ipstr);
    std::cout << "Accepted connection from " << clientIP << std::endl;
    return ClientSocket;
}