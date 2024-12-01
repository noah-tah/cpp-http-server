#include "..\..\include\Utils.h"
#include <fstream>
#include <sstream>
#include <iostream>
namespace Utils {

std::string getContentType(const std::string& filePath) {
    size_t dot = filePath.find_last_of('.');
    if (dot == std::string::npos) {
        return "application/octect-stream";
    }
    std::string extension = filePath.substr(dot + 1);
    if (extension == "html" || extension == "htm") return "text/html";
    if (extension == "css") return "text/css";
    if (extension == "js") return "application/javascript";
    if (extension == "png") return "image/png";
    if (extension == "jpg" || extension == "jpeg") return "image/jpeg";
    if (extension == "gif") return "image/gif";
    return "application/octect-stream";
}

std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        return "";
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
    void log(const std::string& message) {
        std::cout << "\n------------------------------\n" << message << "\n------------------------------\n";
    }

}