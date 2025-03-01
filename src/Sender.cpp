#include "Sender.hpp"

Sender::Sender(void) : client_fd_(-1)
{

}

Sender::Sender(int client_fd) :client_fd_(client_fd)
{

}

Sender::~Sender(void)
{

}

Sender::Sender(const Sender& copy)
{
	this->client_fd_ = copy.client_fd_;
}

Sender& Sender::operator=(const Sender& src)
{
	if (this != &src)
		this->client_fd_ = src.client_fd_;
	return (*this);
}

void	Sender::SendMessage(ChannelResult message, int ew) const
{
	ssize_t	byte_send;
	(void)ew;
	if (client_fd_ == -1)
	{
		std::cerr << "Error: send" << std::endl;
		return ;
	}
	message += "\r\n";
	byte_send = send(client_fd_, message.c_str(), message.length(), 0);
	if (byte_send == -1)
		std::cerr << "Error: send" << std::endl;
}
