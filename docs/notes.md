
# Code 
```C++
int resolveLocalAddress() {
    configureSocketHints(&hints);

    // Resolve local address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        std::cout << "getaddrinfo failed " << iResult << std::endl;
        WSACleanup();
        return 1;
    } else {
        std::cout << "Memory address of the linked list of addrinfo structures containing the resolved addresses: " << result << std::endl;
    }

    struct sockaddr_in *serverBinaryAddress = (struct sockaddr_in *)result->ai_addr;
    std::cout << "Server binary address: " << serverBinaryAddress << std::endl;
    std::cout << "Server Binary address with an &: " << &serverBinaryAddress << std::endl;
    struct sockaddr_in* resultIP = (struct sockaddr_in *)result->ai_addr;
    std::cout << "resultIP: " << resultIP << std::endl;
    std::cout << "&resultIP " << &resultIP << std::endl;

    return 0;
}
```

# Output
```
startWSA() called! Program has begun...
Initializing Winsock Library... Running WSAStartup
WinSock Description: WinSock 2.0
Memory address of the linked list of addrinfo structures containing the resolved addresses: 01231B68
Server binary address: 01234800
Server Binary address with an &: 00F3FD64
resultIP: 01234800
&resultIP 00F3FD60
```
# Explanation of Logged Values
1. Memory address fo the linked list of addrinfo structures containing the resolved addresses: `result`
    - This is the memory address of the first `addrinfo` structure in the linked list returned by `getaddrinfo`
    - This structure contains information about the resolved address, such as the address family, socket type, protocol, and the actual address.
2. Server Binary Address: `serverBinaryAddress`
    - This is the memory address of the `sockaddr_in` structure pointed to by `result->ai_addr`.
    - This structure contains the binary representation of the server's IP address and port
3. Server Binary Address wotj am &: `&serverBinaryAddress`
    - This is the memory address of the pointer variable `serverBinaryAddress` itself. It shows where the pointer variable is stored in memory, not the address it points to

