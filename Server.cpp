#inlcude "Server.h"
#include <stdexcept>

Server::Server(int port) : port(port), serverSocket(INVALID_SOCKET) {
    // Initialize Winsock or any neccessary setup here if needed
}

Server::~Server() {
    cleanup(); // Ensure cleanup of resources
}

bool Server::initialize() {
    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed with error: " << result << std::endl;
        return false;
    } 
    return true;
}


bool Server::createSocket() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
        return false;
    }
    return true;
}

bool Server::bindSocket() {
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
        cleanup();
        return false;
    }
    return true;
}


bool Server::listenConnections() {
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
        cleanup();
        return false;
    }
    return true;
}

void Server::acceptConnections() {
    std::cout << "Waiting for client connections..." << std::endl;
    SOCKET clientSocket;
    sockaddr_in clientAddress;
    iont clientSize = sizeof(clientAddress);

    while ((clientSocket = accept(serverSocket, (sockaddr*)&clientAddress, &clientSize)) != INVALID_SOCKET) {
        std::cout << "Client connected!" << std::endl;
        // Handle client connection here

        closesocket(clientSocket); // Close the socket after handling
    }
}

void Server::cleanup() {
    if (serverSocket != INVALID_SOCKET) {
        closesocket(serverSocket);
        WSACleanup();
    }
}