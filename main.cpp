#include "include/Server.h"
#include "include/Utils.h"
#pragma comment(lib, "Ws2_32.lib")
int main() {
    std::string port = "8080";
    Server server(port);
    Utils::log("Starting server on port " + port);
    server.run();
    return 0;
}