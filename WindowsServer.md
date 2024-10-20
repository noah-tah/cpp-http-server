# Functions of our Windows Server

1. Initialize WSA - WSAStartup()
2. Create a socket - socket()
3. Bind the socket - bind()
4. Listen on the socket - listen()
5. Accept a connection - accept(), connect()
6. Send and receive data - recv(), send(), recvfrom(), sendto()
7. Disconnect - closesocket()


# Functions of the Client
1. Initialize WSA - WSAStartup()
2. Create a socket - socket()
3. Connect to the server - connect()
4. Send and receive data - recv(), send(), recvfrom(), sendto()
5. Disconnect - closesocket()


# The Server Code
The server must load the DLL by invoking WSAStartup
It then creates a socket specifying the protocol to be used
It binds the server's IP address to the socket
Then listens for clients trying to establish connectinos
On a client connecting to the server creates a new socket to handle the client server connection


# Initializing The DLL
1. Initiate use of the Winsock DLL by a process

```c++
int WSAStartup(WORD wVersionRequested,
                LPWSADATA lpWSAData);
```

2. wVersionRequested
    The highest version of windows sockets specification that the caller can use. The higher-order byte specifices the minor version number; the lower byte specifices the major version number
3. A pointer to the LPWSADATA data structure that is to receive details of the Windows Sockets implementation
4. If successful, the WSAStartup function returns zero.

NOTE
---
WORD and LPWSDATA are WINDOWS DATA TYPES (do your research)


# WSData Strucutre
The WSDATA structure contains information about the Windows Sockets Implementation

```c++
typedef struct WSAData {
    WORD wVersion;
    Word wHighVersion;
    char szDescription[WSADESCRIPTION_LEN+1];
    char szSystemStatus[WSASYS_STATUS_LEN+1];
    unsigned short iMaxSockets;
    unsigned short iMaxIdpDg;
    char FAR *IpVendorInfo;
}
```
The WSAStartup function returns a pointer to the WSADATA structure in the IpWSAData parameter


# Accepting a connection
---

```C++
ClientSocket = INVALID_SOCKET;

// Accept a client socket
ClientSocket = accept(ListenSocket, NULL, NULL);
if (ClientSocket == INVALID_SOCKET) {
    printf("accept failed: %d\n", WSAGetLastError());
    closesocket(ListenSocket);
    WSACleanup();
    return 1;
}

```
When the client connection has been accepted, a server application would normally pass the accepted client socket (the ClientSocket variable in the above sample code) to a worker thread or an I/O  completion port and continue accepting additional connections. In this basic example, the server continues to the next step.



