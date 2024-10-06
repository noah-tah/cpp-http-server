#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

class HTTPServer {
        int fd, newSocket;
        struct sockaddr_in address;
    int addressLength;
    const int PORT = 8080;


public:
    int HttpServer() {
        addressLength = sizeof(address);
        //  Creating a socket file descriptor
        if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            std::cerr << "Socket failed" << std::endl;
            std::exit(EXIT_FAILURE);
        }


        //  Set the socket options (optional)
        int opt = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
            std::cerr << "setsockopt failed" << std::endl;
            exit(EXIT_FAILURE);
        }

        //  Define address
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        //  Bind the socket to the port
        if (bind(fd, (struct sockaddr *)&address, sizeof(address)) < 0 ) {
            std::cerr << "Bind failed" << std::endl;
            exit(EXIT_FAILURE);
        }


        //  Start listening on the socket
        if(listen(fd, 3) < 0) {
            std::cerr << "Listen failed" << std::endl;
            exit(EXIT_FAILURE);

        }


        std::cout<< "HTTP Server started on port " << PORT << std::endl;


        //  Accept and handle connectinos
        void handleRequest() {
            char buffer[30000] = {0};
            while(true){
                std::cout << "Waiting for connection..." << std::endl;


                //  Accedpt the incoming connectin
                if ((newSocket = accept(fd, (struct sockaddr *)&address, (socklen_t*)&addressLength)) < 0) {

                }
            }
        };
};




int main() {



}