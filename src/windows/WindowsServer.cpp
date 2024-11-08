// ---------------------------//
// Noah Tah 
// Northern Oklahoma College
// C++ Progamming
// ---------------------------//
// ---------------------------//
// Description:
// This program is a simple HTTP server
// that listens on port 8080 and sends a
// simple HTML response to the client.
// ---------------------------//
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <thread>
#include <atomic>
#include <string>

#pragma comment(lib, "Ws2_32.lib") // Link with Ws2_32.lib (Windows Sockets Library)
#define DEFAULT_PORT "8080"
WSADATA wsaData;
struct addrinfo* result = NULL; 
struct addrinfo hints; 
int iResult; 
SOCKET ListenSocket = INVALID_SOCKET;
std::atomic<bool> running(true); 

void log(const std::string& message);

int configureSocketHints(struct addrinfo* hints) {
    memset(hints, 0, sizeof(struct addrinfo));
    ZeroMemory(hints, sizeof(*hints)); 
    hints->ai_family = AF_INET; 
    hints->ai_socktype = SOCK_STREAM; 
    hints->ai_protocol = IPPROTO_TCP;
    hints->ai_flags = AI_PASSIVE;
    return 0; 
}

int resolveLocalAddress() {
    configureSocketHints(&hints);

    // Resolve local address and port
    iResult = getaddrinfo(NULL,DEFAULT_PORT,&hints,&result);
    if (iResult != 0) {
        std::cout << "getaddrinfo failed " << iResult << std::endl;
        WSACleanup();
        return 1;
    } else {
        std::cout << "Memory address of the linked list of addrinfo structures containing the resolved addresses: " << result << std::endl;
    }

    struct sockaddr_in *serverBinaryAddress = (struct sockaddr_in *)result->ai_addr;
    struct sockaddr_in* resultIP = (struct sockaddr_in *)result->ai_addr;
    // std::string serverIP = extractIPv4(&resultIP);
    // std::cout << serverIP << std::endl;

    return 0;
}

int initializeSocket() {
    ListenSocket = socket(result->ai_family,result->ai_socktype,result->ai_protocol);
    std::cout << "Initialied ListenSocket!" << std::endl;
    if(ListenSocket == INVALID_SOCKET) {
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    } else {
        std::cout << "Socket initialized succesfully!" << std::endl;
        std::cout << "Socket bound with ai_family: " << result->ai_family << ", " << "ai_socktype: " << result->ai_socktype << ", " << "ai_protocol: " << result->ai_protocol << std::endl;
    }
    return 0;
}

int startWSA() {
    std::cout << "startWSA() called! Program has begun..." << std::endl;
    std::cout<< "Initializing Winsock Library... Running WSAStartup" << std::endl;
    iResult = WSAStartup(MAKEWORD(2,2),&wsaData);    
    if (iResult != 0) {
        std::cout << "WSAStartup failed, returned: "<< iResult << std::endl;
        return 1;
    } else {
        std::cout << "WinSock Description: " << wsaData.szDescription << std::endl;
    }
    return 0;
}

int bindSocket() {
    int iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cout << "Bind failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    } else {
        freeaddrinfo(result); 
    }
    return 0;
}

std::string extractIPv4(struct sockaddr_in *ipv4) {
    std::cout << "Extract IPv4 function running now" << std::endl;
    std::cout << "Extracting IPv4 address..." << std::endl;
    char ipstr[INET_ADDRSTRLEN]; 
    if (inet_ntop(AF_INET,&(ipv4->sin_addr),ipstr, INET_ADDRSTRLEN) == NULL) {
        std::cerr << "Failed to convert IPv4 address to string." << std::endl;
    }
    return std::string(ipstr);
}

int listenOnSocket() {
    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "Listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    } else {
        std::cout << "Socket is listening..." << std::endl;
    }
    return 0;
}

SOCKET acceptConnection() {
    SOCKET ClientSocket = INVALID_SOCKET;

    // Buffer struct to hold client information
    struct sockaddr_in clientInfo;
    int clientInfoSize = sizeof(clientInfo);

    ClientSocket = accept(ListenSocket, (struct sockaddr*)&clientInfo, &clientInfoSize); 
    if (ClientSocket == INVALID_SOCKET) {
        std::cout << "Failed to accept socket! " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return INVALID_SOCKET;
    } else {
        extractIPv4(&clientInfo);
        std::cout << "This is after extractIPv4, before inet_ntop" << std::endl;
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)&clientInfo;
        char ipstr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
        std::cout << "Client connected with IP Address: " << ipstr << std::endl;
    }
    return ClientSocket; 
}

int handleRequests(SOCKET ClientSocket) {
    char recvbuf[512];
    int iResult;
    int recvbuflen = 512;
    iResult = recv(ClientSocket, recvbuf, recvbuflen, 0); 
    if (iResult > 0) {
        std::cout << "Bytes received: " << iResult << std::endl;
        const char* httpResponse = "HTTP/1.1 200 OK\rr\nContent-Type: text/html\r\\n\r\n<html><body><h1>Hello, World!</h1></body></html>";
        send(ClientSocket, httpResponse, strlen(httpResponse), 0);
    } else if (iResult == 0) {
        std::cout << "0 Received: Connection closing..." << std::endl;
    } else {
        std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
    }
    closesocket(ClientSocket);
    return 0;
}

void cleanup() {
    closesocket(ListenSocket);
    WSACleanup();
}

void serverLoop() {
    SOCKET ClientSocket = INVALID_SOCKET;
    while (running) {
        if ((ClientSocket = acceptConnection()) == INVALID_SOCKET) {
            std::cout << "Failed to accept connection, INVALID SOCKET!" << std::endl;
            cleanup();
            running = false;
            return;
        } else {
            std::cout << "Connection accepted!" << std::endl;
            handleRequests(ClientSocket);
        }
    
    }
}

int createServerSocket() {
    if (resolveLocalAddress() != 0) return 1;
    if (initializeSocket() != 0) return 1; 
    if (bindSocket() != 0) return 1;
    if (listenOnSocket() != 0);

    // std::cout << "Socket created and ready to listen on port " << DEFAULT_PORT << "!" <<std::endl;

    return 0;
}

int main () {
    // Start the Windows Sockets API
    if (startWSA() != 0) return 1; 

     // Make a Server Socket, and then Listen for connections 
    if (createServerSocket() != 0) return 1;


    std::thread serverThread(serverLoop);

    serverLoop();
    std::cout << "Press Enter to shut down the server..." << std::endl;
    std::cin.get();
    running = false;


    closesocket(ListenSocket);

    serverThread.join();
    // // Run the server
    // while (running) {
    //     serverLoop();
    // }

    WSACleanup();
    return 0;
}
