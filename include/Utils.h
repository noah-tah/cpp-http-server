#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace Utils {
    std::string getContentType(const std::string& filePath);
    std::string readFile(const std::string& filePath);
    void log(const std::string& message);
}
#endif