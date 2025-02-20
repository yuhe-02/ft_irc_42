#include "SocketServer.hpp"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>\n";
        return 1;
    }

    int port = std::atoi(argv[1]);
    if (port <= 0) {
        std::cerr << "Invalid port number.\n";
        return 1;
    }

    std::string password = argv[2];
    SocketServer server(port, password);
    server.Start();

    return 0;
}
