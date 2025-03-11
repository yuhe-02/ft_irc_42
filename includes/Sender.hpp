#ifndef __SENDER_HPP__
#define __SENDER_HPP__

# include <iostream>
# include <sys/socket.h>
# include <iomanip>
# include <string>
# include <sstream>
#include "ErrorCode.h"
#include "Channel.h"
#include "Everyone.h"

class Everyone;

class Sender
{
	private:
		IntrusivePtr<Everyone>	user_;
		std::string createResponse(ChannelResult &result, int fd) const;
		ssize_t sendResponse(std::string &response, int fd) const;
	public:
		Sender(void);
		~Sender(void);
		Sender(const Sender& copy);
		Sender& operator=(const Sender& src);
		void SendMessage(ChannelResult result, int fd) const;
};

#endif
