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
#include <thread>
#include <atomic>
#include <string>
//
#pragma comment(lib, "Ws2_32.lib") // tells linker this library file is needed
//
// End Libraries & setup
// ---------------------- //
//
// Global Definitions
// ---------------------- //

#define DEFAULT_PORT "8080"
WSADATA wsaData;
struct addrinfo* result = NULL; // intializing pointer variable to NULL called result
struct addrinfo hints;
int iResult;
SOCKET ListenSocket = INVALID_SOCKET; 
std::atomic<bool> running(true); // Atomic flag for graceful shutdown

void log(const std::string& message); 
int startWSA();
int createSocket();
void serverLoop();
void cleanup();
int initializeSocketPresets();
int extractIPv4();
int bindSocket();
int listenOnSocket();
void printSocketCreatedSuccess();
int configureSocketHints(struct addrinfo* hints);
SOCKET acceptConnection();
int handleRequests(SOCKET ClientSocket);


//
//
// End Global Definitions
// -------------------------------//




//
// Function Definitions
//


//
// Function setting the addressinfo struct to the desired values
//

int configureSocketHints(struct addrinfo* hints) {

    // Zero out the memory of the vairable hints, which is a pointer to the addrinfo struct
    ZeroMemory(hints, sizeof(*hints)); 

    // Set the address family to IPv4
    hints->ai_family = AF_INET;

    // Set the socket type to stream socket (TCP)
    hints->ai_socktype = SOCK_STREAM;

    // Set the protocol to TCP
    hints->ai_protocol = IPPROTO_TCP;

    // The socket address will be used in the call to bind()
    hints->ai_flags = AI_PASSIVE;


   return 0; 
}

void printSocketCreatedSuccess() {
    std::cout << "Socket created succesfully!" << std::endl;
    std::cout << "Socket bound with ai_family: " << result->ai_family << std::endl;
    std::cout << "Socket bound with ai_socktype: " << result->ai_socktype << std::endl;
    std::cout << "Socket bound with ai_protocol: " << result->ai_protocol << std::endl;
}



int initializeSocketPresets() {

    // Set up the address info struct to store the address information
    configureSocketHints(&hints);

    //
    // Resolve the local address and port to be used by the server
    // - We use NULL for the node paramter to indicate that we want to use a local address 
    // - We use DEFAULT_PORT for the service paramter to specify the port number
    // - We use the hints struct to specify the desired properties of the socket 
    // - The result parameter will point to a linked list of addrinfo structures containing the resolved address 
    //
    // iResult just contains the integer status code
    //

    iResult = getaddrinfo(NULL,DEFAULT_PORT,&hints,&result);

    if (iResult != 0) {
        std::cout << "getaddrinfo failed " << iResult << std::endl;
        WSACleanup();
        return 1;
    } else {
        std::cout << "Memory address of the addrinfo structure containing the resolved address: " << result << std::endl;
    }
    return 0;
}


int extractIPv4() {
    std::cout << "Extracting IPv4 address..." << std::endl;

    struct sockaddr_in *ipv4 = (struct sockaddr_in *)result->ai_addr;
    char ipstr[INET_ADDRSTRLEN]; // Buffer to store IP addr in string

    // Convert the binary IP addr to a string
    inet_ntop(AF_INET,&(ipv4->sin_addr),ipstr,sizeof(ipstr));

    std::cout << "Here is the resolved IP Address " << ipstr << std::endl;
    return 0;
}

int createSocket() {
    // Create a socket using the address information from getaddrinfo which was stored in the result variable
    // This is possible because the result variable points to the linked list of addrinfo structures containing the resolved address
    ListenSocket = socket(result->ai_family,result->ai_socktype,result->ai_protocol);
    std::cout << "Initialied ListenSocket!" << std::endl;

    // Check if the socket creation failed
    if(ListenSocket == INVALID_SOCKET) {
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);  // Free the address information
        WSACleanup(); // Clean up Winsock
        return 1;
    } else {
        printSocketCreatedSuccess(); // Print success message
    }

    return 0;
}

void welcomeToProgram () {
    std::cout << "Program has begun..." << std::endl;
    std::cout << "Initializing Winsock... Running WSAStartup" << std::endl;
}

//
// Run WSAstartup to initialize Winsock
//

int startWSA() {
    welcomeToProgram();


    iResult = WSAStartup(MAKEWORD(2,2),&wsaData);    

    if (iResult != 0) {
        std::cout << "WSAStartup failed, returned: "<< iResult << std::endl;
        return 1;
    } else {
        std::cout << "WinSock Description: " << wsaData.szDescription << std::endl;
    }

    return 0;

}

//
// Bind the socket to the address and port specified in the addrinfo data structure
//

int bindSocket() {

    // Setup the TCP listening socket


    int iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cout << "Bind failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    } else {
        std::cout << "Socket bound!" << std::endl;
        freeaddrinfo(result);
    }
    return 0;
}

//
// Listen on the socket, taking in the socket and the backlog which is the maximum number of connections that can be queued 
//

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

//
// Accept Connections
//

SOCKET acceptConnection() {


    SOCKET ClientSocket = INVALID_SOCKET;

    // Data structure to store client information
    struct sockaddr_in clientInfo;

    // This the size of sockaddr_in struct we use to store client information
    int clientInfoSize = sizeof(clientInfo);

    //
    // The addr parameter in the accept() function can point to a sockaddr structure
    // That will be filled with the address of the client that is connecting
    //

    ClientSocket = accept(ListenSocket, (struct sockaddr*)&clientInfo, &clientInfoSize); 
    if (ClientSocket == INVALID_SOCKET) {
        std::cout << "Failed to accept socket! " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return INVALID_SOCKET;
    } else {

        // Extract the IPv4 address of the client
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

        // Process the received data (for simplicity, we assume it's an HTTP GET request)
        // Send an HTTP rersponse
        const char* httpResponse = "HTTP/1.1 200 OK\rr\nContent-Type: text/html\r\\n\r\n<html><body><h1>Hello, World!</h1></body></html>";
        send(ClientSocket, httpResponse, strlen(httpResponse), 0);
    } else if (iResult == 0) {
        std::cout << "0 Received: Connection closing..." << std::endl;
    } else {
        std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
    }


    // Close the client socket
    closesocket(ClientSocket);
    return 0;
}

void cleanup() {
    closesocket(ListenSocket);
    WSACleanup();
}

void serverLoop() {
    while (running) {
        SOCKET ClientSocket = acceptConnection();
        if (ClientSocket == INVALID_SOCKET) break;
        std::thread(handleRequests, ClientSocket).detach();
    }
}

//
// End Function Definitions
//



// Main Server Code
//----------------------//

// # Functions of our Windows Server

// 1. Initialize WSA - WSAStartup()
// 2. Create a socket - socket()
// 3. Bind the socket - bind()
// 4. Listen on the socket - listen()
// 5. Accept a connection - accept(), connect()
// 6. Send and receive data - recv(), send(), recvfrom(), sendto()
// 7. Disconnect - closesocket()


int main () {

    if (startWSA() != 0) return 1; 
    if (initializeSocketPresets() != 0) return 1;
    if (createSocket() != 0) return 1; 
    if (extractIPv4() != 0) return 1;
    if (bindSocket() != 0) return 1;
    if (listenOnSocket() != 0) return 1;

    // std::thread serverThread(serverLoop);


    // closesocket(ListenSocket);
    // serverThread.join();
    // cleanup();


    // while (true) {
    //     SOCKET ClientSocket = acceptConnection();
    //     if (ClientSocket == INVALID_SOCKET) break;
    //     handleRequests(ClientSocket);
    // }

    SOCKET ClientSocket = INVALID_SOCKET;

    // Accept single client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        std::cout << "Accept failed: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }




    WSACleanup();
    return 0;
}

// ----------------------//
// End Main Server Code
//----------------------//