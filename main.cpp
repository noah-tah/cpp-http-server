#include "include/Server.h"
#include "include/Utils.h"

int main() {
    std::string port = "8080";
    Server server(port);
    Utils::log("Starting server on port " + port);
    server.run();
    return 0;
}