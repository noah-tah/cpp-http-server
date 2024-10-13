#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
/*
    We must be sure that the build environment links to the Winsock Library file
    The #pragma comment indicates to the linkers that the library file is needed
*/

#pragma comment(lib, "Ws2_32.lib") // tells linker this library file is needed


// Global Definitions
WSADATA wsaData;

struct addrinfo hints;
struct addrinfo* result = NULL;
int iResult;


int initializeHints() {
    #define DEFAULT_PORT "27015"

    struct addrinfo *result = NULL, *ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        std::cout << "getaddrinfo failed " << iResult << std::endl;
        WSACleanup();
        return 1;
    } else {
        std::cout << "intializeHints() was successful!" << std::endl;
        return 0;
    }

}

int startEngines() {
    std::cout << "Program has begun..." << std::endl;
    std::cout << "Initializing Winsock... Running WSAStartup" << std::endl;

    //
    // All processess that call winsock functions must initialize the use of the Windows Sockets
    // DLL before making other winsock functions
    //
    // Initialize Winsock
    //

    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed: "<< iResult << std::endl;
        return 1;
    } else {
        std::cout << "WSAStartup ran successfully! " << std::endl;
    }


}
int main () {

    startEngines(); // Run that function to get stuff started right
    initializeHints(); // Call the function to set up hints



    return 0;
}