#include <sstream>
#include <iostream>
#include "..\..\include\HTTPRequest.h"

bool HTTPRequestParser::parse(const std::string& requestData, HTTPRequest& request) {
    std::istringstream requestStream(requestData);
    std::string line;

    // Parse the request line  
    if (!std::getline(requestStream, line) || line.empty()) {
        return false;
    }

    std::istringstream requestLineStream(line);
    if(!(requestLineStream >> request.method >> request.path >> request.version)) {
        return false;
    }

    // Parse headers  
    while (std::getline(requestStream, line) && !line.empty()) {
        if (line.back() == '\r' ) {
            line.pop_back();
        }
        size_t delimiterPos = line.find(':');
        if (delimiterPos != std::string::npos) {
            std::string headerName = line.substr(0, delimiterPos);
            std::string headerValue = line.substr(delimiterPos + 1);
            headerValue.erase(0, headerValue.find_first_not_of(" \t"));
            request.headers[headerName] = headerValue;
        }
    }

    // Parse body if Content-Length is specified
    auto it = request.headers.find("Content-Length");
    if (it != request.headers.end()) {
        int contentLength = std::stoi(it->second);
        std::string body(contentLength, '\0');
        requestStream.read(&body[0], contentLength);
        request.body = body;
    }

    return true;
}



