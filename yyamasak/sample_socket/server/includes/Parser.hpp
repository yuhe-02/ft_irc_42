#ifndef __PARSER_HPP__
# define ___PARSER_HPP__

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
#include "./RefCounted.h"
#include "./IntrusivePtr.h"

struct ClientInfo
{
	std::string nick;
	std::string user;
	bool hasNick;
	bool hasUser;
	bool registered;
};

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
		void action(const std::string &str);

};
# endif