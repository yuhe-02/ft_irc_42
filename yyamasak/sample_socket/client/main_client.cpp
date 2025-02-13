#include "SocketClient.hpp"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <port>\n";
        return 1;
    }

    std::string server_ip = argv[1];
    int port = std::atoi(argv[2]);

    SocketClient client(server_ip, port);
    client.run();

    return 0;
}