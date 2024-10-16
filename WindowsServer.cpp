// ---------------------------//
// Noah Tah 
// Northern Oklahoma College
// C++ Progamming
// ---------------------------//
// Libraries & Setup
//
//
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <windows.h>
//
#pragma comment(lib, "Ws2_32.lib") // tells linker this library file is needed
//
// End Libraries & setup
// ---------------------- //
//
// Global Definitions
// ---------------------- //

#define DEFAULT_PORT "27015"
WSADATA wsaData;
struct addrinfo* result = NULL; // intializing pointer variable to NULL called result
struct addrinfo hints;
int iResult;
//
//
// End Global Definitions
// -------------------------------//




void printSocketCreatedSuccess() {
    std::cout << "Socket created succesfully!" << std::endl;
    std::cout << "Socket bound with ai_family: " << result->ai_family << std::endl;
    std::cout << "Socket bound with ai_socktype: " << result->ai_socktype << std::endl;
    std::cout << "Socket bound with ai_protocol: " << result->ai_protocol << std::endl;
}

int configureSocketHints(struct addrinfo* hints) {
    ZeroMemory(hints, sizeof(*hints)); // Zero out the memory of the variable hints, which is a pointer to the arrinfo struct
    hints->ai_family = AF_INET; // IPv4 address family
    hints->ai_socktype = SOCK_STREAM; // Stream socket
    hints->ai_protocol = IPPROTO_TCP; // TCP protocol
    hints->ai_flags = AI_PASSIVE; // caller intends to use the returned socket address structure in a call to bind()


   return 0; 
}

int initializeSocket() {
    configureSocketHints(&hints);

    iResult = getaddrinfo(
        NULL, // Hostname to be resolved ; Null means local machine
        DEFAULT_PORT, // We defined this earlier 
        &hints, // point to our struct addrinfo which holds the hints
        &result // point to our struct addrinfo which contains the addrinfo structure that matches hints criteria. result is used to create a socket
    );

    if (iResult != 0) {
        std::cout << "getaddrinfo failed " << iResult << std::endl;
        WSACleanup();
        return 1;
    } else {
        std::cout << "getaddressinfo was a success, returned: " << iResult << std::endl;
    }
    return 0;
}

int createSocket() {
    SOCKET ListenSocket = INVALID_SOCKET; 

    std::cout << "Initialied ListenSocket: " << ListenSocket << std::endl;

    ListenSocket = socket(
        result->ai_family,
        result->ai_socktype,
        result->ai_protocol
    );

    if(ListenSocket == INVALID_SOCKET) {
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    } else {
        printSocketCreatedSuccess();
    }
    return 0;
}

void welcomeToProgram () {
    std::cout << "Program has begun..." << std::endl;
    std::cout << "Initializing Winsock... Running WSAStartup" << std::endl;
}


int startWSA() {
    welcomeToProgram();

    iResult = WSAStartup(
        MAKEWORD(2,2), // Major and Min Version
        &wsaData  // pointer to a WSADATA struct where to store Windows Socket implementation 
    );    

    if (iResult != 0) {
        std::cout << "WSAStartup failed, returned: "<< iResult << std::endl;
        return 1;
    } else {
        std::cout << "WSAStartup ran successfully! " << std::endl;
    }

    return 0;

}


int main () {

    if (startWSA() != 0) return 1; 
    if (initializeSocket() != 0) return 1;
    if (createSocket() != 0) return 1; 


    freeaddrinfo(result);
    WSACleanup();
    return 0;
}