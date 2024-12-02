#include "..\..\include\HTTPResponse.h"
#include <sstream>

std::string HTTPResponse::constructResponse(const std::string& status, const std::string& contentType, const std::string& content) {
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 " << status << "\r\n"
                   << "Content-Type: " << contentType << "\r\n"
                   << "Content-Length: " << content.length() << "\r\n"
                   << "Connection: close\r\n\r\n"
                   << content;
    return responseStream.str();
};