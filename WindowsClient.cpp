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
int main() {

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
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        std::cout << "getaddrinfo failed, returned: " << iResult << std::endl;
        WSACleanup();
        return 1;
    }

    // Create a SOCKET object called ConnectSocket
    SOCKET ConnectSocket = INVALID_SOCKET;

    // Call the socket function and return its value to the ConnectSocket variable.
    // For this application, use the first IP returned by the call to getaddrinfo
    // This IP matched the address family, socket type, and protocol specified in
    // the hints parameter.
    // This is a TCP socket (SOCK_STREAM) so we use the IPPROTO_TCP parameter.
    // The address family we used is AF_INET, which is the IPv4 address family.

    // Attempt to connect to the first address returned by the call to getaddrinfo
    ptr = result;

    // Create a SOCKET for connecting to server
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        std::cout << "Error at socket(), returned: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    




   return 0;
}


