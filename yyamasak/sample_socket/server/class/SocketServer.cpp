#include "../includes/SocketServer.hpp"

void SocketServer::setNonBlocking(int fd) {
	fcntl(fd, F_SETFL, O_NONBLOCK);
}

bool SocketServer::initializeServer() {
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	int opt = 1;
	struct sockaddr_in server_addr;
	struct pollfd pfd;

	if (server_fd == -1) {
		perror("socket");
		return false;
	}
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	std::memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);
	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
		perror("bind");
		return false;
	}
	if (listen(server_fd, MAX_CLIENTS) == -1) {
		perror("listen");
		return false;
	}
	setNonBlocking(server_fd);
	pfd.fd = server_fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	poll_fds.push_back(pfd);
	std::cout << "Server listening on port " << port << "...\n";
	return true;
}

void SocketServer::handleNewConnection() {
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
	struct pollfd pfd;

	if (client_fd < 0) {
		perror("accept");
		return;
	}
	setNonBlocking(client_fd);
	pfd.fd = client_fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	poll_fds.push_back(pfd);
    auth_map[client_fd] = false;

	std::cout << "New client connected: " << inet_ntoa(client_addr.sin_addr)
			  << ":" << ntohs(client_addr.sin_port) << "\n";
}

void SocketServer::handleClientMessage(size_t index)
{
	char buffer[BUFFER_SIZE];
	std::memset(buffer, 0, BUFFER_SIZE);
	int client_fd = poll_fds[index].fd;
	int bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

	if (bytes_read > 0) {
		buffer[bytes_read] = '\0';
		std::cout << "Client [" << client_fd << "] says: " << buffer << std::endl;

		if (!auth_map[client_fd]) {
            if (send(client_fd, "You are not allowed to aceess", 30, 0) == -1) {
                perror("send");
            }
        } else {
            if (send(client_fd, buffer, bytes_read, 0) == -1) {
                perror("send");
            }
        }
	} else {
		closeClient(index);
	}
}

void SocketServer::closeClient(size_t index)
{
	std::cout << "Client disconnected: FD " << poll_fds[index].fd << "\n";
	close(poll_fds[index].fd);
	poll_fds.erase(poll_fds.begin() + index);
}

SocketServer::SocketServer(int port, const std::string &password) : port(port), password(password), server_fd(-1) {}

SocketServer::~SocketServer() {
	cleanup();
}

void SocketServer::start() {
	if (!initializeServer()) {
		return;
	}

	while (true) {
		int ret = poll(poll_fds.data(), poll_fds.size(), -1);
		if (ret < 0) {
			perror("poll");
			break;
		}
		for (size_t i = 0; i < poll_fds.size(); i++) {
			if (poll_fds[i].revents & POLLIN) {
				if (poll_fds[i].fd == server_fd) {
					handleNewConnection();
				} else {
					handleClientMessage(i);
				}
			}
		}
	}
}

void SocketServer::cleanup() {
	for (size_t i = 0; i < poll_fds.size(); i++) {
		close(poll_fds[i].fd);
	}
	std::cout << "Server shutdown.\n";
}
