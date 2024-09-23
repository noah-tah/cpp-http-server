#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>



int main() {
    int  mySocketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << "\n" << "The file descriptor is: " << mySocketFileDescriptor << "\n";

    bind(3, sockaddr )


}