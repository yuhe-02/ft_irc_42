#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <cstdio>

#define BUFFER_SIZE 512
#define MAX_CLIENTS 10

void set_nonblocking(int fd) {
    fcntl(fd, F_SETFL, O_NONBLOCK);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>\n";
        return 1;
    }

    int port = std::atoi(argv[1]);
    if (port <= 0) {
        std::cerr << "Invalid port number.\n";
        return 1;
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, MAX_CLIENTS) == -1) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    set_nonblocking(server_fd);

    std::vector<struct pollfd> poll_fds;
    poll_fds.push_back((struct pollfd){server_fd, POLLIN, 0});

    std::cout << "Server listening on port " << port << "...\n";

    while (true) {
        int ret = poll(&poll_fds[0], poll_fds.size(), -1);
        if (ret < 0) {
            perror("poll");
            break;
        }

        for (size_t i = 0; i < poll_fds.size(); i++) {
            if (poll_fds[i].revents & POLLIN) {
                if (poll_fds[i].fd == server_fd) {
                    // 新しいクライアントの接続を受け付ける
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
                    if (client_fd < 0) {
                        perror("accept");
                        continue;
                    }

                    set_nonblocking(client_fd);
                    poll_fds.push_back((struct pollfd){client_fd, POLLIN, 0});

                    std::cout << "New client connected: " << inet_ntoa(client_addr.sin_addr)
                              << ":" << ntohs(client_addr.sin_port) << "\n";
                } else {
                    // クライアントからのデータ受信
                    char buffer[BUFFER_SIZE];
                    std::memset(buffer, 0, BUFFER_SIZE);
                    int bytes_read = recv(poll_fds[i].fd, buffer, BUFFER_SIZE - 1, 0);

                    if (bytes_read > 0) {
                        buffer[bytes_read] = '\0'; // 文字列の終端を追加
                        std::cout << "Client [" << poll_fds[i].fd << "] says: " << buffer << std::endl;

                        // クライアントにメッセージを送り返す
                        if (send(poll_fds[i].fd, buffer, bytes_read, 0) == -1) {
                            perror("send");
                        }
                    } else if (bytes_read == 0) {
                        // クライアントが切断
                        std::cout << "Client disconnected: FD " << poll_fds[i].fd << "\n";
                        close(poll_fds[i].fd);
                        poll_fds.erase(poll_fds.begin() + i);
                        i--; // erase したので index 調整
                        continue;
                    } else {
                        // エラーが発生
                        perror("recv");
                    }
                }
            }
        }
    }

    // クリーンアップ
    for (size_t i = 0; i < poll_fds.size(); i++) {
        close(poll_fds[i].fd);
    }
    return 0;
}
