// ---------------------------//
// Noah Tah 
// Northern Oklahoma College
// C++ Progamming
// ---------------------------//

// ---------------------------//
// Libraries & Setup
// ---------------------------//

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <thread>
#include <atomic>
#include <string>

// Link with Ws2_32.lib (Windows Sockets Library)
#pragma comment(lib, "Ws2_32.lib") 

//----------------------//
// End Libraries & setup
//----------------------//

//----------------------//
// Global Definitions
//----------------------//

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


// -------------------------------//
// End Global Definitions
// -------------------------------//



//----------------------//
//Function Definitions
//----------------------//


/*
Function setting the addressinfo struct to the desired values
Zero out the memory of the hints, which is a pointer to the adddrinfo struct
Set the address family to a family containg IPv4, and set socket type to stream socket (TCP)
Set the socket type to stream socket (TCP)
Set the protocol to TCP
AI_PASSIVE means the socket address will be called to bind()
*/

int configureSocketHints(struct addrinfo* hints) {
    ZeroMemory(hints, sizeof(*hints)); 
    hints->ai_family = AF_INET;
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_protocol = IPPROTO_TCP;
    hints->ai_flags = AI_PASSIVE;
    return 0; 
}

void printSocketCreatedSuccess() {
    std::cout << "Socket created succesfully!" << std::endl;
    std::cout << "Socket bound with ai_family: " << result->ai_family << std::endl;
    std::cout << "Socket bound with ai_socktype: " << result->ai_socktype << std::endl;
    std::cout << "Socket bound with ai_protocol: " << result->ai_protocol << std::endl;
}

/*
Set up address info struct to store address information
Resolve the local address and port to be used by the server
- We use NULL for the node parameter to indicate that we want to use a local address
- We use DEFAULT_PORT for the service parameter to specify the port number
- We use the hints struct to specify the desired properties of the socket
- The result parameter will point to a linked list of addrinfo structures containing the resolved address
iResult just contains the integer status code
*/

int initializeSocketPresets() {
    configureSocketHints(&hints);
    iResult = getaddrinfo(NULL,DEFAULT_PORT,&hints,&result);
    if (iResult != 0) {
        std::cout << "getaddrinfo failed " << iResult << std::endl;
        WSACleanup();
        return 1;
    } else {
        std::cout << "Memory address of the linked list of addrinfo structures containing the resolved addresses: " << result << std::endl;
    }
    return 0;
}

/*
Extract the result 
- The result variable points to the linked list of addrinfo structures containing the resolved client addresses
- We can extract the IPv4 address from the first addrinfo structure in the linked list
- We cast the result->ai_addr to a sockaddr_in pointer to access the sin_addr field which contains the binary IP address
- We use inet_ntop to convert the binary IP address to a string and store it in the ipstr buffer
- ipstr is the buffer to store the IP address in string form
- INET_ADDRSTRELEN is a constant that specifies the size of a buffer for an IPv4 address in string form
- &(ipv4->sin_addr) is the binary IP address
*/

int extractIPv4() {
    std::cout << "Extracting IPv4 address..." << std::endl;
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)result->ai_addr;
    char ipstr[INET_ADDRSTRLEN]; // Buffer to store IP addr in string
    inet_ntop(AF_INET,&(ipv4->sin_addr),ipstr,sizeof(ipstr));
    std::cout << "Here is the resolved IP Address " << ipstr << std::endl;
    return 0;
}

/*
Create a socket using the address information from getaddrinfo which was stored in the result variable
This is possible because the result variable points to the linked list of addrinfo structures
containing the resolved addresses
*/

int createSocket() {
    ListenSocket = socket(result->ai_family,result->ai_socktype,result->ai_protocol);
    std::cout << "Initialied ListenSocket!" << std::endl;
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

/*
Run WSAstartup to initialize Winsock
*/

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

/*
Bind the socket to the address and port specified in the addrinfo data structure
*/ 
int bindSocket() {
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

/*
Listen on the socket, taking in the socket and the backlog which is the maximum number of connections that can be queued 
*/
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

/*
Accept Connections
create sockaddr_in data strcuture to store client information
call accept() to accept the connection
    s: socket we are listening on
    addr: pointer to a sockaddr structure that will be filled with the address of the client that is connecting
    addrlen: pointer to an int that will be filled with the size of the address of the client that is connecting
if accept() returns a valid socket, then we have a connection
    print the IP address of the client
    return the client socket        
*/
SOCKET acceptConnection() {
    SOCKET ClientSocket = INVALID_SOCKET;
    struct sockaddr_in clientInfo;
    int clientInfoSize = sizeof(clientInfo);
    ClientSocket = accept(ListenSocket, (struct sockaddr*)&clientInfo, &clientInfoSize); 
    if (ClientSocket == INVALID_SOCKET) {
        std::cout << "Failed to accept socket! " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return INVALID_SOCKET;
    } else {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)&clientInfo;
        char ipstr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
        std::cout << "Client connected with IP Address: " << ipstr << std::endl;
    }
    return ClientSocket;
}

/*
used recv() to receive data from the client
if recv() returns a positive value, then we have received data
    print the number of bytes received
    process the received data
    send an HTTP response
*/

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

/*
This cleanup function closes the socket,
then calls WSACleanup to clean up Winsock which will unload the Winsock 2.2 DLL which will free the memory allocated by WSAStartup
*/

void cleanup() {
    closesocket(ListenSocket);
    WSACleanup();
}

/*
This loop function will run as long as the running flag is set to true which is an atomic boolean
which is set to true by default which means the server will run until the running flag is set to false
which will happen when the server is shutting down which will be done by the main function which will set the running flag to false
which will cause the server loop to break out of the loop and the server will shut down which will call the cleanup function
which will close the socket and clean up Winsock
*/

void serverLoop() {
    while (running) {
        SOCKET ClientSocket = acceptConnection();
        if (ClientSocket == INVALID_SOCKET) break;
        std::thread(handleRequests, ClientSocket).detach();
    }
}


//----------------------//
// End Function Definitions
//----------------------//


//----------------------//
// Main Server Code
//----------------------//

/*
Functions of our Windows Server
    1. Initialize WSA - WSAStartup()
    2. Create a socket - socket()
    3. Bind the socket - bind()
    4. Listen on the socket - listen()
    5. Accept a connection - accept(), connect()
    6. Send and receive data - recv(), send(), recvfrom(), sendto()
    7. Disconnect - closesocket()
*/

int main () {
    if (startWSA() != 0) return 1; 
    if (initializeSocketPresets() != 0) return 1;
    if (createSocket() != 0) return 1; 
    if (extractIPv4() != 0) return 1;
    if (bindSocket() != 0) return 1;
    if (listenOnSocket() != 0) return 1;
    SOCKET ClientSocket = INVALID_SOCKET;
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