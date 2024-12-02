#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <string>
#include <map>

struct HTTPRequest {
    std::string method; // GET, POST, etc.
    std::string path; // Requested path
    std::string version; // HTTP version
    std::map<std::string, std::string> headers;
    std::string body;
};
class HTTPRequestParser {
public: 
    bool parse(const std::string& requestData, HTTPRequest& request);
};

#endif