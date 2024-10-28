#include "Server.h"

int main() {
    int port = 8080;
    Server server(port);

    if (!server.initialize()) {
        return -1;
    }

    if (!server.createSocket()) {
        return -1;
    }

    if (!server.bindSocket()) {
        return -1;
    }

    if (!server.listenConnections() {
        return -1;
    })

    server.acceptConnections();
    return 0;
}