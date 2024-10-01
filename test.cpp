#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>


int main() {
    //  Step 1: Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) {
        std::cerr << "Failed to create socket!" << std::endl;
        return 1;
    }

    // Step 2: Define the address structure (sockaddre_in)
    struct sockaddr_in myAddress; // This is our strucutre
    memset(&myAddress, 0, sizeof(myAddress));

    //  Step 3: Fill in the details of the structure
    myAddress.sin_family = AF_INET;
    myAddress.sin_port = htons(8080);
    myAddress.sin_addr.s_addr = inet_addr("127.0.0.1");


    //  Step 4: Bind the socket to the address
    //  Cast sockaddr_in* to sockaddr*
    if (bind(sockfd, (struct sockaddr*)&myAddress, sizeof(myAddress)) == -1) {
            std::cerr << "Failed to bind!" << std::endl;
            close(sockfd); // close the socket before returning
            return 1;
    }

    std::cout << "Socket succesffully bound!" << std::endl;


    //  Step 5: Close the socket (after use)
    close(sockfd);

    return 0;
}