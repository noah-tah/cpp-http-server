//
// Noah Tah 
// Northern Oklahoma College
// C++ Progamming
//
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <windows.h>


// Global Definitions
#define DEFAULT_PORT "27015"
#pragma comment(lib, "Ws2_32.lib") // tells linker this library file is needed
WSADATA wsaData;
struct addrinfo* result = NULL; // intializing pointer variable to NULL called result
struct addrinfo hints;
int iResult;


//
// Call the socket function to return its value to the ListenSocket variable (object)
// use the first IP address returned by the call to getaddrinfo that matched the address
// family, socket type, and protocol specified in the hints parameter
//
//
// in this example, a TCP stream socket for IPv4 was requested with an address family of
// IPv4, a socket type of SOCK_STREAM and a protocol of IPPROTO_TCP. so an IPv4 address
// is requested for the ListenSocket


void printSocketCreatedSuccess() {
    std::cout << "Socket created succesfully!" << std::endl;
    std::cout << "Socket bound with ai_family: " << result->ai_family << std::endl;
    std::cout << "Socket bound with ai_socktype: " << result->ai_socktype << std::endl;
    std::cout << "Socket bound with ai_protocol: " << result->ai_protocol << std::endl;
}

int configureSocketHints(struct addrinfo* hints) {
    ZeroMemory(hints, sizeof(*hints)); // Zero out the memory of the variable hints, which is a pointer to the arrinfo struct
    hints->ai_family = AF_INET;
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_protocol = IPPROTO_TCP;
    hints->ai_flags = AI_PASSIVE;


   return 0; 
}

int initializeSocket() {
    configureSocketHints(&hints);


    // Resolve the local address and port to be used by the server
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
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

    //
    // All processess that call winsock functions must initialize the use of the Windows Sockets
    // DLL before making other winsock functions
    //


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