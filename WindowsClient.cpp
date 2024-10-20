#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>


#pragma comment(lib, "Ws2_32.lib") // tells linker this library file is needed
//
// Client Code Main Functionality
//
// 1. Initialize Winsock
// 2. Create a socket
// 3. Connect to the server
// 4. Send and receive data
// 5. Disconnect


// Call WSAStartup and return its value as an integer and check for errors.
//
// The WSAStartup function is called to initiate the use of WS2_32.dll

int startWSA() {
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
}
int main() {
    
}


