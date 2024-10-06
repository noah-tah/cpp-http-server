#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>


class HttpServer {
private:
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen;
    const int PORT = 8080;

public:
    HttpServer() {
        addrlen = sizeof(address);
        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            std::cerr << "Socket failed" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Set the socket options (optional)
        int opt = 1;
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
            std::cerr << "setsockopt failed" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Define address
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        // Bind the socket to the port
        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            std::cerr << "Bind failed" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Start listening on the socket
        if (listen(server_fd, 3) < 0) {
            std::cerr << "Listen failed" << std::endl;
            exit(EXIT_FAILURE);
        }

        std::cout << "HTTP Server started on port " << PORT << std::endl;
    }

    // Accept and handle connections
    void handleRequest() {
        char buffer[30000] = {0};
        while (true) {
            std::cout << "Waiting for a connection..." << std::endl;

            // Accept the incoming connection
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                std::cerr << "Accept failed" << std::endl;
                exit(EXIT_FAILURE);
            }

            // Read the request
            read(new_socket, buffer, 30000);
            std::cout << "Request received:\n" << buffer << std::endl;

            // Prepare the response
            std::string http_response = "HTTP/1.1 200 OK\n"
                                        "Content-Type: text/html\n"
                                        "Content-Length: 53\n\n"
                                        "<html><body><h1>Hello, World!</h1></body></html>";

            // Send the response
            send(new_socket, http_response.c_str(), http_response.size(), 0);
            std::cout << "Response sent" << std::endl;

            // Close the socket for this connection
            close(new_socket);
        }
    }
};

int main() {
    HttpServer server;
    server.handleRequest(); // Start listening and serving HTTP requests
    return 0;
}
