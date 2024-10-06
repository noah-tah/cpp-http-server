#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>


class HttpServer {
private :
    int serverFd, newSocket = -1;
    struct sockaddr_in address;
    int addressLength;
    const int PORT = 8080;

public:
    HttpServer () {
        addressLength = sizeof(address);

        //Creating socket file descriptor
        if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            std::cerr << "Socket failed" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Set the socket options (optional)
        int opt = 1;
        if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
            std::cerr << "setsockopt failed" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Define address
        address.sin_family = AF_INET; // Address family that contains IPv4
        address.sin_addr.s_addr = INADDR_ANY; // Accepts any incoming connections
        address.sin_port = htons(PORT);

        // Bind the socket to the port
        if (bind(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            std::cerr << "Bind failed" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Start listening on the socket
        if (listen(serverFd, 3) < 0) {
            std::cerr << "Listen failed" << std::endl;
            exit(EXIT_FAILURE);
        }

        std::cout << "HTTP Server started on port " << PORT << std::endl;
    }


    // Accept and handle connections
    void handleRequest() {
        char buffer[30000] = {0};
        while (true) {
            std::cout << "Waiting for a connectrion..." << std::endl;

            // Accept the incoming connection
            if ((newSocket = accept(serverFd, (struct sockaddr *)&address, (socklen_t*)&addressLength)) < 0 ) {
                std::cerr << "Failed to accept connection" << std::endl;
                exit(EXIT_FAILURE);
            } 

            // Read the request
            read(newSocket, buffer, 30000);
            std::cout << "Request received: \n" << buffer << std::endl;

            // Prepare the response
            std::string httpResponse = "HTTP/1.1 200 OK\n"
                                        "Content-Type: text/html\n"
                                        "Content-Length: 53\n\n"
                                        "<html><body><h1>Hello, World!</h1></body></html>";

            // Send the response
            send(newSocket, httpResponse.c_str(), httpResponse.size(), 0);
            std::cout << "Response sent" << std::endl;

            // Close the socket for this connection
            close(newSocket);
        }
    }
};



int main() {
    HttpServer server;
    server.handleRequest();
    return 0;
};


