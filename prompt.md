SOCKET acceptConnections(SOCKET ListenSocket) {
    // Create a socket to hold the clinet connection
    SOCKET ClientSocket = INVALID_SOCKET;

    // Buffer struct to hold client information
    struct sockaddr_in clientInfo;
    int clientInfoSize = sizeof(clientInfo);

    std::cout << "Please open a web browser and connect to: http://localhost:8080/ " << std::endl;

    // Accept a connection from a client
    ClientSocket = accept(ListenSocket, (struct sockaddr*)&clientInfo, &clientInfoSize); 
    if (ClientSocket == INVALID_SOCKET) {
        int error = WSAGetLastError();
        std::cout << "Failed to accept connection on socket! " << error << std::endl;

        // Handle specific errors
        if (error == WSAEINTR || error == WSAECONNRESET) {
            // continue;
        } else {
            std::cout << "Critical error, closing socket and exiting..." << std::endl;
            closesocket(ListenSocket);
            WSACleanup();
            running = false;
            return INVALID_SOCKET;
        }

        
        closesocket(ListenSocket);
        WSACleanup();
        running = false;
        return INVALID_SOCKET;
    } 

    // Extract the IP address from the clientInfo struct, then print it
    std::string ipstr = extractIPv4(&clientInfo);
    std::cout << "Client connected with IP Address: " << ipstr << std::endl;

    return ClientSocket; 
}

std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int handleRequests(SOCKET ClientSocket) {

    // Create the buffer which will be used to store the data received from the client
    const int recvbuflen = 1024;
    char recvbuf[recvbuflen];

    // iResult will contain the number of bytes received from the client
    int iResult;
    
    // Receive data from the client
    iResult = recv(ClientSocket, recvbuf, recvbuflen, 0); 

    // If there are bytes received, print the number of bytes received, and send a response to the client
    if (iResult > 0) {



        // Convert the received data to a string
        std::string requestData(recvbuf, iResult);
        // Print the raw HTTP request
        std::cout << "Received: "  << prettyLineBreak << requestData << std::endl;
        
        // Create a HTTPRequest object to store the parsed request 
        HTTPRequest request;

        // Parse the request, and log the results
        if (parseRequest(requestData, request)) {
            std::cout << "Parsed request:" << std::endl;
            std::cout << "Method: " << request.method << std::endl;
            std::cout << "Path: " << request.path << std::endl;
            std::cout << "Version: " << request.version << std::endl;
            std::cout << "Host: " << request.headers["Host"] << std::endl;
            std::cout << "Content-Length: " << request.headers["Content-Length"] << std::endl;
            std::cout << "Body: " << request.body << std::endl;
        } else {
            std::cout << "Failed to parse request" << std::endl;
            std::string httpResponse = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<html><body><h1>400 Bad Request</h1></body></html>";
            send(ClientSocket, httpResponse.c_str(), httpResponse.length(), 0);
            closesocket(ClientSocket);
            return 0;
        }

        std::string filePath;
        if (request.path == "/" || request.path == "/index.html") {
            filePath = "index.html";
        } else if (request.path == "/contact") {
            filePath = "contact.html";
        } else if (request.path == "/about") {
            filePath = "about.html";
        } else if (request.path == "/projects") {
            filePath = "projects.html";
        } else {
            std::string httpResponse = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<html><body><h1>400 Bad Request</h1></body></html>";
            send(ClientSocket, httpResponse.c_str(), httpResponse.length(), 0);
            closesocket(ClientSocket);
            return 0;
        }
        
        // Read the HTML File onto the server
        std::string content = readFile(filePath);
        if (content.empty()) {
            std::string httpResponse = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n<html><body><h1>500 Internal Server Error</h1></body></html>";
            send(ClientSocket, httpResponse.c_str(), httpResponse.length(), 0);
            closesocket(ClientSocket);
            return 0;
        }


        how do i start to sever the html files i have, they are contained back one directory, and then inside of a pages directory, where index.html, contact.html,
        about.html, projects.html are held.

        how far away from that am i? I am updating the file path so far based on what is being parsed in the path field on the http request but my problem now is that
        I am not able to read the file i think? What do you think is going wrong.

    