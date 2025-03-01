#ifndef __SENDER_HPP__
#define __SENDER_HPP__

# include <iostream>
# include <sys/socket.h>
#include "ErrorCode.h"
#include "Channel.h"
#include "Everyone.h"

class Sender
{
	private:
		int				client_fd_;
	public:
		Sender(void);
		Sender(int fd);
		~Sender(void);
		Sender(const Sender& copy);
		Sender& operator=(const Sender& src);
		void SendMessage(ChannelResult result);
};

#endif