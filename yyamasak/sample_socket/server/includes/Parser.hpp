#ifndef __PARSER_HPP__
# define __PARSER_HPP__

#include <string>
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
#include <cerrno>
#include <sstream>
#include <utility>
#include "RefCounted.h"
#include "IntrusivePtr.h"
#include "ErrorCode.h"
#include "SocketServer.hpp"

class SocketServer;
struct ClientInfo;

class Parser : public RefCounted {
	private:
		static IntrusivePtr<Parser> instance_;
		std::map<int, ClientInfo> clients_;
	public:
		Parser();
		Parser(const Parser &other);
		Parser &operator=(const Parser &other);
		~Parser();
		static IntrusivePtr<Parser>	GetInstance();
		void registerClient(int client_fd);
		void action(const std::string &line, int client_fd, const std::string &password);
		bool isExist(int client_fd);

};
# endif