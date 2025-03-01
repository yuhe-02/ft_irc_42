#include "Sender.hpp"

Sender::Sender(void)
{

}

Sender::~Sender(void)
{

}

Sender::Sender(const Sender& copy)
{
	(void)copy;
}

Sender& Sender::operator=(const Sender& src)
{
	(void)src;
	return (*this);
}

void	Sender::SendMessage(ChannelResult result, int fd)
{
	ssize_t		byte_send;
	std::string	message;

	if (fd < 0)
	{
		std::cerr << "Error: send" << std::endl;
		return ;
	}
	message += result.second;
	std::cerr << "Message: [" << result.second << "]" << std::endl;//デバッグ用
	if (!result.second.empty())
		message += "\r\n";
	// message += "\r\n";
	byte_send = send(fd, message.c_str(), message.length(), 0);
	if (byte_send == -1)
		std::cerr << "Error: send" << std::endl;
}