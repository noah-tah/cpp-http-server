link: https://learn.microsoft.com/en-us/windows/win32/winsock/finished-server-and-client-code
------------------------------------------------------|
# Running the Winsock Client and Server Code Sample
|-----------------------------------------------------|
------------------------------------------------------|
# The server application should be started before the client application is started
------------------------------------------------------|
- To execute the server, compile the complete server source code and run the executable
file.
- The server application listens on TCP port 27015 for a client to connect.
- Once a client connects, the server receives data from the client and echoes
the data received back to the client.
- When the client shuts down the connection, the server shuts down the client socket, closes the socket, and exits.

# To execute the Client
----------------------|
- Compile the complete client source code and run the executable file.
- The client application requires that name of the computer or IP address of the computer where the server application is running is passed as a command-line parameter when the client is executed.
- If the client and server are execute on the sample computer, the client can be started as follows:

# To execute if Client and Server are on the same computer
-----------------------------------------------------------|
```c++
client localhost
```

The client tries to connect to the server on TCP port 27015. Once the client connects, the client sends data to the server and receives any data sent back from the server. The client then closes the socket and exits

|-----------------------------------------------------|
