#include "..\..\include\Server.h"
#include "..\..\include\Networking.h"
#include "..\..\include\HTTPRequest.h"
#include "..\..\include\HTTPResponse.h"
#include "..\..\include\Utils.h"
#include <thread>


Server::Server(const std::string& port) : port(port), listenSocket(INVALID_SOCKET) {
    Networking networking;
    listenSocket = networking.createListenSocket(port);
}

Server::~Server() {
    if (listenSocket != INVALID_SOCKET) {
        closesocket(listenSocket);
    }
}

void Server::run() {
    Networking networking;
    while (true) {
        std::string clientIP;
        SOCKET clientSocket = networking.acceptClient(listenSocket, clientIP);
        if (clientSocket == INVALID_SOCKET) {
            Utils::log("Failed to accept client.");
            continue;
        }

        // Handle client in a seperate thread
        std::thread(&Server::handleClient, this, clientSocket).detach();
    }
}

void Server::handleClient(SOCKET clientSocket) {
    const int recvbuflen = 2048;
    char recvbuf[recvbuflen];
    int iResult = recv(clientSocket, recvbuf, recvbuflen, 0);

    if (iResult > 0) {
        std::string requestData(recvbuf, iResult);
        Utils::log("Received HTTP Request:\n" + requestData);

        HTTPRequestParser parser;
        HTTPRequest request;
        if (parser.parse(requestData, request)) {
            Utils::log("Parsed HTTPRequest:\nMethod: " + request.method + "\nPath: " + request.path);
            
            std::string baseDirectory = "../pages/";
            std::string filePath;

            if(request.path == "/") {
                filePath = baseDirectory + "index.html";
            } else {
                // Remove leading '/'
                if (request.path.front() == '/') {
                    filePath = baseDirectory + request.path.substr(1);
                } else {
                    filePath = baseDirectory + request.path;
                }
            }
            std::string content = Utils::readFile(filePath);
            if (content.empty()) {
                std::string notFound = "<html><body><h1>404 Not Found</h1></body></html>";
                std::string response = HTTPResponse::constructResponse("404 Not Found", "text/html", notFound);
                send(clientSocket, response.c_str(), response.length(), 0);
                Utils::log("Sent 404 Not Found for " + filePath);
            } else {
                std::string contentType = Utils::getContentType(filePath);
                std::string response = HTTPResponse::constructResponse("200 OK", contentType, content);
                send(clientSocket, response.c_str(), response.length(), 0);
                Utils::log("Sent 400 Bad Request");
            }
        } else if (iResult == 0) {
            Utils::log("Connection closing...");
        } else {
            Utils::log("recv failed with error: " + std::to_string(WSAGetLastError()));
        }

        closesocket(clientSocket);
    }
}