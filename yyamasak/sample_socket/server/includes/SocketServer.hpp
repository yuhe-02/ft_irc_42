#ifndef __SOCKETSERVER_HPP__
# define __SOCKETSERVER_HPP__

#include <iostream>
#include <map>
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

class SocketServer {
	private:
		int server_fd;
		int port;
		std::string password;
		std::vector<struct pollfd> poll_fds;
		std::map<int, bool> auth_map;
		void setNonBlocking(int fd);
		bool initializeServer();
		void handleNewConnection();
		void handleClientMessage(size_t index);
		void closeClient(size_t index);
	public:
		SocketServer(int port, const std::string& password);
		~SocketServer();
		void start(); 
		void cleanup();
};
#endif