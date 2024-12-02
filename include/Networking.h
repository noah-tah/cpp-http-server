
// include guards used to prevent multiple inclusions of the same header file=

// checks if NETWORKING_H has been defined
#ifndef NETWORKING_H // if not defined, the code between #ifndef & #endif will be included
#define NETWORKING_H // Ensures next time this header file is included, #ifndef will be false, preventing reinclusion
#include <string>
#include <WS2tcpip.h>
#include <winsock2.h>



class Networking {
public:
    Networking();
    ~Networking();

    SOCKET createListenSocket(const std::string& port);
    SOCKET acceptClient(SOCKET listenSocket, std::string& clientIP);

private:
    WSADATA wsaData;
    void initializeWinsock();
    void cleanup();
};

#endif // Marks the end of the code block that should be included only once