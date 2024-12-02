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
#include <map> 
#include <string> 
#include <sstream> 
#include <iostream> 
#include <xtree> 
#include <fstream>

#pragma comment(lib, "Ws2_32.lib") // Link with Ws2_32.lib (Windows Sockets Library)
#define DEFAULT_PORT "8080"
WSADATA wsaData;

int iResult; 
std::atomic<bool> running(true); 
std::string prettyLineBreak = "\n---------------------------\n";

void log(const std::string& message);

// Configure the socket hints for the getaddrinfo function
void configureSocketHints(struct addrinfo* hints) {
    memset(hints, 0, sizeof(struct addrinfo));
    ZeroMemory(hints, sizeof(*hints)); 
    hints->ai_family = AF_INET; 
    hints->ai_socktype = SOCK_STREAM; 
    hints->ai_protocol = IPPROTO_TCP;
    hints->ai_flags = AI_PASSIVE;
    return;
}

// Define the structure of the HTTPrequest
struct HTTPRequest {
    std::string method; // GET, POST, PUT, DELETE
    std::string path; // index.html
    std::string version; // HTTP/1.1
    std::string host; // localhost:8080
    std::map<std::string, std::string> headers; // key-value pairs
    std::string body;
};

// Parse the HTTP request
bool parseRequest(const std::string& requestData, HTTPRequest& request) {
    // Create a string stream from the request data
    std::istringstream requestStream(requestData);
    std::string line;

    // Parse the request line
    if (!std::getline(requestStream, line) || line.empty()) {
        return false;
    }
    std::istringstream requestLineStream(line);
    if (!(requestLineStream >> request.method >> request.path >> request.version)) {
        return false;
    }

    // Parse headers
    while (std::getline(requestStream, line) && !line.empty()) {
        if (line.back() == '\r') { // Remove the carriage return character if it exists
            line.pop_back();  // std::string::pop_back() removes the last character of the string
        }
        // Find the position of the delimiter which seperates the key and value
        size_t delimiterPos = line.find(':');
        // if the delimiter is found then add the key-value pair to the headers map
        if (delimiterPos != std::string::npos) { // std::string::npos is the maximum value representable by the size_t type
            std::string headerName = line.substr(0, delimiterPos); // Extract the key
            std::string headerValue = line.substr(delimiterPos + 1); // Extract the value
            headerValue.erase(0, headerValue.find_first_not_of(" \t")); // Remove leading whitespace
            request.headers[headerName] = headerValue; // Add the key-value pair to the headers map
        }
    }

    // Parse body if Content-Length is specified in the headers
    auto it = request.headers.find("Content-Length"); // Find the Content-Length header
    if (it != request.headers.end()) { // Check if the Content-Length header exists
        int contentLength = std::stoi(it->second); // std::stoi converts a string to an integer, it->second is the value of the Content-Length header
        std::string body(contentLength, '\0'); // Create a string of length contentLength filled with null characters
        requestStream.read(&body[0], contentLength); // Read contentLength bytes from the requestStream into the body string
        request.body = body; // Set the body of the request to the body string
    }

    return true;
}
struct addrinfo* resolveLocalAddress() {
    // Declare a pointer to an addrinfo struct
    struct addrinfo* result = NULL;

    // Declare a struct to hold the hints for the getaddrinfo function
    struct addrinfo hints;

    // Configure the hints for the getaddrinfo function
    configureSocketHints(&hints);

    // Resolve local address and port
    iResult = getaddrinfo(NULL,DEFAULT_PORT,&hints,&result); // getaddrinfo returns 0 on success
    if (iResult != 0) {
        std::cout << "getaddrinfo failed " << iResult << std::endl;
        WSACleanup();
        return nullptr;
    } 

    return result;
}

SOCKET initializeSocket(struct addrinfo* result) {
    // Create a socket that will listen for incoming connections
    SOCKET ListenSocket = INVALID_SOCKET;

    // Create a socket using the information from the addrinfo struct `result`
    ListenSocket = socket(result->ai_family,result->ai_socktype,result->ai_protocol); // socket() returns INVALID_SOCKET on failure, otherwise a new socket descriptor

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
    iResult = WSAStartup(MAKEWORD(2,2),&wsaData); // MAKEWORD(2,2) requests version 2.2 of Winsock on Windows, &wsaData is a pointer to a WSADATA struct
    if (iResult != 0) {
        std::cout << "WSAStartup failed, returned: "<< iResult << std::endl;
        return 1;
    } else {
        std::cout << "WinSock Description: " << wsaData.szDescription << std::endl; // wsaData.szDescription is a char array containing a null-terminated string describing the Windows Sockets implementation
    }
    return 0;
}

int bindSocket(SOCKET ListenSocket, struct addrinfo* result) {
    // Bind the socket to the local addres and port
    int iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen); // result is a pointer to an addrinfo struct, which contains the local address and port
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
    // Convert the IP address to a string
    char ipstr[INET_ADDRSTRLEN]; // INET_ADDRSTRLEN is the maximum length of an IPv4 address string
    if (inet_ntop(AF_INET,&(ipv4->sin_addr),ipstr, INET_ADDRSTRLEN) == NULL) { // inet_ntop converts an IPv4 or IPv6 address to a string
        std::cerr << "Failed to convert IPv4 address to string." << std::endl;
    }
    return std::string(ipstr);
}

int listenOnSocket(SOCKET ListenSocket) {
    // Listen on the socket for incoming connections
    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) { // SOMAXCONN is the maximum number of pending connections that can be queued
        std::cout << "Listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    } else {
        std::cout << "Socket is listening..." << std::endl;
    }
    return 0;
}

SOCKET acceptConnections(SOCKET ListenSocket) {
    // Create a socket to hold the clinet connection
    SOCKET ClientSocket = INVALID_SOCKET;

    // Buffer struct to hold client information
    struct sockaddr_in clientInfo;
    int clientInfoSize = sizeof(clientInfo);

    std::cout << "Please open a web browser and connect to: http://localhost:8080/ " << std::endl;

    // Accept a connection from a client
    ClientSocket = accept(ListenSocket, (struct sockaddr*)&clientInfo, &clientInfoSize); // accept() returns INVALID_SOCKET on failure, otherwise a new socket descriptor
    if (ClientSocket == INVALID_SOCKET) {
        int error = WSAGetLastError();
        std::cout << "Failed to accept connection on socket! " << error << std::endl;

        // Handle specific errors
        if (error == WSAEINTR || error == WSAECONNRESET) { // WSAEINTR is the error code for a blocking call being interrupted, WSAECONNRESET is the error code for a connection being reset
            // continue;
        } else {
            std::cout << "Critical error, closing socket and exiting..." << std::endl;
            closesocket(ListenSocket);
            WSACleanup();
            running = false;
            return INVALID_SOCKET;
        }

        
        closesocket(ListenSocket);
        WSACleanup();
        running = false;
        return INVALID_SOCKET;
    } 

    // Extract the IP address from the clientInfo struct, then print it
    std::string ipstr = extractIPv4(&clientInfo);
    std::cout << "Client connected with IP Address: " << ipstr << std::endl;

    return ClientSocket; 
}

std::string readFile(const std::string& filePath) {
    // Open the file
    std::ifstream file(filePath); // ifstream is a class used to read from files
    if (!file.is_open()) {
        return "";
    }
    std::stringstream buffer; // stringstream is a class used to manipulate strings
    buffer << file.rdbuf(); // rdbuf() reads the entire file into the buffer
    return buffer.str(); // str() returns the contents of the buffer as a string
}
std::string getContentType(const std::string& filePath) {
    size_t dot = filePath.find_last_of('.');
    if (dot == std::string::npos) {
        return "application/octet-stream";
    }
    std::string fileExtension = filePath.substr(dot + 1);
    if (fileExtension == "html" || fileExtension == "htm") return "text/html";
    if (fileExtension == "css") return "text/css";
    if (fileExtension == "js") return "application/javascript";
    if (fileExtension == "png") return "image/png";
    if (fileExtension == "jpg" || fileExtension == "jpeg") return "image/jpeg";
    if (fileExtension == "gif") return "image/gif";
    if (fileExtension == "css") return "text/css";
    return "application/octet-stream";
}
int handleRequests(SOCKET ClientSocket) {

    // Create the buffer which will be used to store the data received from the client
    const int recvbuflen = 1024;
    char recvbuf[recvbuflen];

    // iResult will contain the number of bytes received from the client
    int iResult;
    
    // Receive data from the client
    iResult = recv(ClientSocket, recvbuf, recvbuflen, 0); // recv() returns the number of bytes received, or -1 on error

    // If there are bytes received, print the number of bytes received, and send a response to the client
    if (iResult > 0) {

        // Convert the received data to a string
        std::string requestData(recvbuf, iResult); 

        // Print the raw HTTP request
        std::cout << "Received the following HTTP request: "  << prettyLineBreak << requestData << prettyLineBreak << std::endl;
        
        // Create a HTTPRequest object to store the parsed request 
        HTTPRequest request;

        // Parse the request, and log the results
        if (parseRequest(requestData, request)) {
            std::cout << "Parsed request:" << std::endl;
            std::cout << "Method: " << request.method << std::endl;
            std::cout << "Path: " << request.path << std::endl;
            std::cout << "Version: " << request.version << std::endl;
            std::cout << "Host: " << request.headers["Host"] << std::endl;
            std::cout << "Content-Length: " << request.headers["Content-Length"] << std::endl;
            std::cout << "Body: " << request.body << std::endl;
        } else {
            std::cout << "Failed to parse request" << std::endl;
            std::string httpResponse = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<html><body><h1>400 Bad Request</h1></body></html>";
            send(ClientSocket, httpResponse.c_str(), httpResponse.length(), 0);
            closesocket(ClientSocket);
            return 0;
        }

        // Specify the base directory where HTML files are stored
        std::string baseDirectory = "../pages/";

        // Specify the filePath baed on the parsed path from the HTTP request
        std::string filePath;
        if (request.path == "/" || request.path == "/index.html") {
            filePath = baseDirectory + "index.html";
        } else if (request.path == "/contact.html") {
            filePath = baseDirectory + "contact.html";
        } else if (request.path == "/about.html") {
            filePath = baseDirectory + "about.html";
        } else if (request.path == "/projects.html") {
            filePath = baseDirectory + "projects.html";
        } else {
            std::string httpResponse = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<html><body><h1>400 Bad Request</h1></body></html>";
            send(ClientSocket, httpResponse.c_str(), httpResponse.length(), 0);
            closesocket(ClientSocket);
            return 0;
        }
        
        // Read the HTML file content 
        std::string content = readFile(filePath);
        if (content.empty()) {
            std::string httpResponse = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n<html><body><h1>500 Internal Server Error</h1></body></html>";
            send(ClientSocket, httpResponse.c_str(), httpResponse.length(), 0);
            closesocket(ClientSocket);
            return 0;
        }

        // Create the HTTP response with the file content
        std::string httpResponse = "HTTP/1.1 200 OK\r\n"
                                   "Content-Type: text/html\r\n"
                                   "Content-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;

        // Log the complete HTTP response for debugging
        std::cout << prettyLineBreak << "The HTTP Reponse we are sending: " << httpResponse << prettyLineBreak << std::endl;


        // Send the response to the client
        send(ClientSocket, httpResponse.c_str(), httpResponse.length(), 0); // .c_str() returns a pointer to an array that contains a null-terminated sequence of characters (i.e., a C-string)

        closesocket(ClientSocket);

        std::cout << "Served file: " << filePath << std::endl;

    
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


// TODO: Make this logging less annoying and more useful
void serverLoop(SOCKET ListenSocket) {

    // Run the server
    while (running) {
        SOCKET ClientSocket = INVALID_SOCKET;

        // Accept a connection from a client
        ClientSocket = acceptConnections(ListenSocket); 
        if (ClientSocket == INVALID_SOCKET) {
            std::cout << "Failed to accept connection, INVALID SOCKET!" << std::endl;
            cleanup(ListenSocket);
            running = false;
            return;
        }

        std::cout << "Connection accepted!" << std::endl;

        // Create a new execution thread to handle the requests from the client
        std::thread clientThread(handleRequests, ClientSocket); 
        std::cout << "Client thread ID: " << clientThread.get_id() << std::endl;
        clientThread.detach(); // .detach() allows the thread to run independently
    }
}

SOCKET createServerSocket() {
    // Resolve the local address
    struct addrinfo* result = resolveLocalAddress();
    if (result == nullptr) {
        return INVALID_SOCKET;
    }

    // Create a socket that will listen for incoming connections
    SOCKET ListenSocket = initializeSocket(result);
    if (ListenSocket == INVALID_SOCKET) {
        freeaddrinfo(result);
        return INVALID_SOCKET;
    }
    
    // Bind the socket to the local address and port
    if (bindSocket(ListenSocket, result) != 0) {
        return INVALID_SOCKET;
    }

    // Listen on the socket
    if (listenOnSocket(ListenSocket) != 0) {
        return INVALID_SOCKET;
    }


    std::cout << "Socket created and ready to listen on port " << DEFAULT_PORT << '!' << std::endl;
    return ListenSocket;
}




int main () {
    // Start the Windows Sockets API
    if (startWSA() != 0) return 1; 

    // Make a Server Socket, and then Listen for connections 
    SOCKET ListenSocket = createServerSocket();
    if (ListenSocket == INVALID_SOCKET) return 1;

    // Start the server loop on a new thread
    std::thread serverThread(serverLoop, ListenSocket);
    std::cout << "Server thread ID: "<< serverThread.get_id() << std::endl;

    // Wait for user input to end the server
    std::cin.get();
    running = false;

    // Close the listening socket
    closesocket(ListenSocket);

    serverThread.join();

    WSACleanup();
    return 0;
}
