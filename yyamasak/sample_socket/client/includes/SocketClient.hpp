#ifndef __SOCKETCLIENT_HPP__
# define __SOCKETCLIENT_HPP__

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>

#define BUFFER_SIZE 512

class SocketClient {
	private :
		int client_socket;
		std::string server_ip;
		int port;
	public :
		SocketClient(const std::string& ip, int port);
		~SocketClient();
		bool connectToServer();
		bool sendMessage(const std::string& message);
		std::string receiveMessage();
		void run();
};
#endif
