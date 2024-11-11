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

int iResult; 

void log(const std::string& message);

void configureSocketHints(struct addrinfo* hints) {
    memset(hints, 0, sizeof(struct addrinfo));
    ZeroMemory(hints, sizeof(*hints)); 
    hints->ai_family = AF_INET; 
    hints->ai_socktype = SOCK_STREAM; 
    hints->ai_protocol = IPPROTO_TCP;
    hints->ai_flags = AI_PASSIVE;
    return;
}

struct addrinfo* resolveLocalAddress() {
    // Declare a pointer to an addrinfo struct
    struct addrinfo* result = NULL; 

    // Declare a struct to hold the hints for the getaddrinfo function
    struct addrinfo hints; 

    // Configure the hints for the getaddrinfo function
    configureSocketHints(&hints);

    // Resolve local address and port
    iResult = getaddrinfo(NULL,DEFAULT_PORT,&hints,&result);
    if (iResult != 0) {
        std::cout << "getaddrinfo failed " << iResult << std::endl;
        WSACleanup();
        return nullptr;
    } else {
        std::cout << "Memory address of the linked list of addrinfo structures containing the resolved addresses: " << result << std::endl;
    }

    struct sockaddr_in *serverBinaryAddress = (struct sockaddr_in *)result->ai_addr;
    struct sockaddr_in* resultIP = (struct sockaddr_in *)result->ai_addr;

    return result;
}

SOCKET initializeSocket(struct addrinfo* result) {
    // Create a socket that will listen for incoming connections
    SOCKET ListenSocket = INVALID_SOCKET;

    // socket() returns a socket descriptor that can be used in later function calls that operate on sockets
    ListenSocket = socket(result->ai_family,result->ai_socktype,result->ai_protocol);

    // Log the file descriptor
    std::cout << "Initialied ListenSocket with file descriptor: " << ListenSocket << std::endl;

    // Check if the socket was created successfully
    if(ListenSocket == INVALID_SOCKET) {
        // If the socket was not created successfully, log the error code and return 1

        // Log the error code
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;

        // Free the memory allocated for the addrinfo struct
        freeaddrinfo(result);
        
        // Close the socket
        WSACleanup();
        return 1;
    } else {
        // If the socket was created successfully, log the success message
        std::cout << "Socket initialized succesfully!" << std::endl;
        std::cout << "Socket bound with ai_family: " << result->ai_family << ", " << "ai_socktype: " << result->ai_socktype << ", " << "ai_protocol: " << result->ai_protocol << std::endl;
    }
    return ListenSocket;
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

int bindSocket(SOCKET ListenSocket, struct addrinfo* result) {
    // Bind the socket to the local addres and port
    int iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cout << "Bind failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    } else {
        std:: cout << "Socket bound successfully!" << std::endl;
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

int listenOnSocket(SOCKET ListenSocket) {
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

SOCKET acceptConnection(SOCKET ListenSocket) {
    SOCKET ClientSocket = INVALID_SOCKET;

    // Buffer struct to hold client information
    struct sockaddr_in clientInfo;
    int clientInfoSize = sizeof(clientInfo);

    std::cout << "Please open a web browser and connect to: http://localhost:8080/ " << std::endl;
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
    // Create the buffer which will be used to store the data received from the client
    const int recvbuflen = 1024;
    char recvbuf[recvbuflen];
    // iResult will contain the number of bytes received from the client
    int iResult;
    
    // Receive data from the client, and store it in recvbuf which is 1024 bytes long and stores the data in the form of a char array which is a string in c++
    iResult = recv(ClientSocket, recvbuf, recvbuflen, 0); 

    // If there are bytes received, print the number of bytes received, and send a response to the client
    // When we receive bytes that means the client has sent a request to the server in the form of a string of bytes which we can convert to a string
    if (iResult > 0) {


        std::cout << "Bytes received: " << iResult << std::endl;

        // Convert the received data to a string
        // Here we are calling a constructor of the std::string class, this initializes the std::string object with the data from recvbuf which is a char array of length iResult
        std::string requestData(recvbuf, iResult);

        // Print the received data
        std::cout << "Received: " << requestData << std::endl;

        // Create the reponse to send to the client
        // HTTP response created using std::string which is a c++ class that represents a string of characters
        std::string httpResponse("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello, World!</h1></body></html>", 1024);

        // Send the response to the client
        // we have to convert the std::string to a c-style string using the c_str() method, then we find the length of the string using the length() method
        send(ClientSocket, httpResponse.c_str(), httpResponse.length(), 0);

    
    } else if (iResult == 0) {
        std::cout << "0 Received: Connection closing..." << std::endl;
    } else {
        std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
    }
    closesocket(ClientSocket);
    return 0;
}

void cleanup(SOCKET ListenSocket) {
    closesocket(ListenSocket);
    WSACleanup();
}

void serverLoop(SOCKET ListenSocket, std::atomic<bool>& running) {
    while (running) {
        SOCKET ClientSocket = acceptConnection(ListenSocket);
        if (ClientSocket == INVALID_SOCKET) {
            std::cout << "Failed to accept connection, INVALID SOCKET!" << std::endl;
            cleanup(ListenSocket);
            running = false;
            return;
        } else {
            std::cout << "Connection accepted!" << std::endl;
            handleRequests(ClientSocket);

            // TODO: Fix clientThread
            // std::thread clientThread(handleRequests);
            // clientThread.detach();
        }
    
    }
}

SOCKET createServerSocket() {
    struct addrinfo* result = resolveLocalAddress();
    SOCKET ListenSocket = initializeSocket(result); 
    bindSocket(ListenSocket, result);
    listenOnSocket(ListenSocket);

    // std::cout << "Socket created and ready to listen on port " << DEFAULT_PORT << "!" <<std::endl;

    return ListenSocket;
}

int main () {
    // Start the Windows Sockets API
    if (startWSA() != 0) return 1; 
    std::atomic<bool> running(true); 

     // Make a Server Socket, and then Listen for connections 
    SOCKET ListenSocket = createServerSocket();

    // Represents a single thread of execution
    // std::thread is a class template of the C++ Standard Library that provides a representation of a thread
    // serverThread() is the thread object that will represent the executing serverLoop function
    // We pass in the function severLoop which will be executed by the serverThread
    // We also pass in the socket that we created that will be used to listen for incoming connections
    // Finally, we pass in the atomic boolean running which will be used to control the server loop
    std::thread serverThread(serverLoop, ListenSocket, std::ref(running));
    std::cout << "Server thread ID: "<< serverThread.get_id() << std::endl;



    std::cout << "Press enter to end the server..." << std::endl;
    std::cin.get();
    running = false;

    // The join() function blocks the calling thread until the thread identified by the thread object finishes executing
    serverThread.join();

    // TODO: Load a custom HTML file
    // TODO: Create routing for multiple HTML files

    closesocket(ListenSocket);

    WSACleanup();
    return 0;
}
