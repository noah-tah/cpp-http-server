#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>



int main() {
    int  fd = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << "\n" << "The file descriptor is: " << fd << std::endl;

    std::string addr, len;

    int bind(fd, addr, len);
}


