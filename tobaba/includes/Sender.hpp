#ifndef __SENDER_HPP__
#define __SENDER_HPP__

# include <iostream>
# include <sys/socket.h>
#include "ErrorCode.h"
#include "Channel.h"
#include "Everyone.h"

class Sender
{
	public:
		Sender(void);
		~Sender(void);
		Sender(const Sender& copy);
		Sender& operator=(const Sender& src);
		void SendMessage(ChannelResult result, int fd);
};

#endif