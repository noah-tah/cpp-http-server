#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>


#pragma comment(lib, "Ws2_32.lib") // tells linker this library file is needed
#define DEFAULT_PORT "8080"

// Client Code Main Functionality
//
// 1. Initialize Winsock
// 2. Create a socket
// 3. Connect to the server
// 4. Send and receive data
// 5. Disconnect


// __cdecl is a calling convention that defines how the function should handle
// the stack and arguments. Defauylt calling convention for C and C++ programs
// On x86 architecture

// int argc: An int parameter that represents the number of command-line
// arguments are passed to the program, inlcuding the program name itself
// char **argv: An arrray of C-style character pointers rerpresenting 
// command line arguments
//      argv[0] is the program name
//      argv[1] is the first command line argument provided by the user
int __cdecl main(int argc, char **argv) {

    // Check if the user passed in the serverr address
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " server-name" << std::endl;
        return 1;
    }



    //----------------------//
    // Initializing Winsock
    //----------------------//


    // Create WSADATA object called wsaData
    WSADATA wsaData;

    // Integer result used for error checking
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

    if (iResult != 0) {
        std::cout << "WSAStartup failed, returned: " << iResult << std::endl;
        return 1;
    }

    //----------------------//
    // Create a Client Socket
    //----------------------//
    // Initialize the variables needed to create the socket:
    // 1. A addrinfo structure called result to store a linked list of potential addresses
    // 2. A addrinfo structure called ptr to iterate through the linked list
    // 3. A addrinfo structure called hints to provide hints for getaddrinfo
    struct addrinfo *result = NULL, *ptr = NULL, hints;

    // Clear addrinfo structure's memory
    ZeroMemory(&hints, sizeof(hints));

    // Set up the addrinfo hints structure to specify the desired protocol
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    
    // iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);

    if (iResult != 0) {
        std::cout << "getaddrinfo failed, returned: " << iResult << std::endl;
        WSACleanup();
        return 1;
    }

    SOCKET ConnectSocket = INVALID_SOCKET;
    ptr = result;

    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        std::cout << "Error at socket(), returned: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    //----------------------//
    // Connect to the Server
    //----------------------//
    
    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }

    // Should really try the next address returned by getaddrinfo
    // if the call connect failed
    // But for now we just free the resources 
    // returned by getaddrinfo and print the error message

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        std::cout << "INVALID_SOCKET! Unable to connect to server!" << std::endl;
        WSACleanup();
        return 1;
    }
    
    //----------------------//
    // Send and Receive Data
    //----------------------//

    #define DEFAULT_BUFLEN 512

    int recvbuflen = DEFAULT_BUFLEN;


    const char *sendbuf = "this is a test";
    char recvbuf[DEFAULT_BUFLEN];

    // Send an initial buffer
    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR ) {
        std::cout << "send failed, returned: " << WSAGetLastError() << std::endl;
        closesocket(ConnectSocket);
        return 1;
    }

    std::cout << "Bytes Sent: " << iResult << std::endl;

    // shutdown the connection for sending since no more data will be sent
    // the client can still use the ConnectSocket for receiving data
    iResult = shutdown(ConnectSocket, SD_SEND);
    if(iResult == SOCKET_ERROR) {
        std::cout << "shutdown failed, returned: " << WSAGetLastError() << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Receive data until the server closes the connection

    do {
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            std::cout << "Bytes received: " << iResult << std::endl;
        } else if (iResult == 0 ) {
            std::cout << "Connection closed" << std::endl;
        } else {
            std::cout << "recv failed, returned: " << WSAGetLastError() << std::endl;
        } 
    } while (iResult > 0);

    // Once the client is completed sending and receiving data, close the socket

    // shutdown the send half of the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        std::cout << "shutdown failed, returned: " << WSAGetLastError() << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // When the client is done re3ceving data, closesocket function is called to close socket
    // Then WSACleanup to release resources related to Windows Sockets DLL

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();



   return 0;
}



