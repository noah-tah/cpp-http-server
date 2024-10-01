#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <cstring>

// struct addr = {

// };

int main() {
    
    //  1.  Create the socket
    int  fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        std::cerr << "Failed to create socket!" << std::endl;
        return 1;
    }
    std::cout << "The file descriptor is: " << fd << std::endl;

    //  2.  Define the address
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET; //   address family: IPv4
    addr.sin_port = htons(8080); //     port number
    addr.sin_addr.s_addr = INADDR_ANY; //   bind to any available addr


    //  3.  Bind the socket to the adress and port

    int resultOfBinding = bind(fd, (struct sockaddr*)&addr, sizeof(addr));

    if (resultOfBinding < 0) {
        std::cerr << "Failed to bind socket!" << std::endl;
        return 1;
    } else {
        std::cout << "Socket bound to port 8080!" << std::endl;
    }



    // Learning memset here
    std::string foo = "This is a long string of text, hero";
    std::cout << "I'm not crazy here, right?: " << foo << " " << &foo << std::endl;
    std::cout << "The pointer thingy is this: " << &foo << " The size of is: " << sizeof(foo) << std::endl;
    memset(&foo, 0, sizeof(foo));
    std::cout << "The result after the memset function: " << foo << " The size of is: " << sizeof(foo) << std::endl;

    std::cout << "The size of the general sockaddr: " << sizeof(sockaddr_in) << std::endl;
    std::cout << "The size of the general sockaddr: " << sizeof(sockaddr) << std::endl;

    return 0;
}


