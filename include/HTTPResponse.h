#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include <string>

class HTTPResponse {
public:
    static std::string constructResponse(const std::string& status, const std::string& contentType, const std::string& content);
};

#endif