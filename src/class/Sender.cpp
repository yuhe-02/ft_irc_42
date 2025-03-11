#include "Sender.hpp"

Sender::Sender(void)
{
	user_ = Everyone::GetInstance();
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

std::string Sender::createResponse(ChannelResult &result, int fd) const
{
	std::string	message;
	std::stringstream ss;

	if (result.first != -1)
	{
		std::cout << "result: " << result.second << std::endl;
		// std::cerr << "ChannelResult: " << result.first << ", " << result.second << std::endl;//デバッグ用
		ss << ":localhost " << std::setw(3) << std::setfill('0') << result.first;
		if ((result.first != 451 && result.first != 464) && (result.second.find("PASS") == std::string::npos))
		{
			// unknown command時にニックネームがない可能性があるため
			if (user_->IsCreated(fd))
			{
				ss << " " << user_->GetSomeone(fd).nick_name.back();
			}
		}
		if (result.second != "")
		{
			ss << " " << result.second;
		}
	} else
	{
		ss << result.second;
	}
	ss << "\r\n";
	std::string msg = ss.str();
	return (msg);
}

ssize_t Sender::sendResponse(std::string &response, int fd) const
{
    ssize_t bytes_sent = 0;
    size_t total_len = response.length();
	// 一回のメッセージ送信で全て送れるとは限らないらしく、送れるまでループ
    while (bytes_sent < static_cast<ssize_t>(total_len))
    {
        ssize_t sent = send(fd, response.c_str() + bytes_sent, total_len - bytes_sent, 0);
        if (sent == -1)
        {
            std::cerr << "Error: send" << std::endl;
            return (-1);
        }
        bytes_sent += sent;
    }
	return (bytes_sent);
}

void	Sender::SendMessage(ChannelResult result, int fd) const
{
	if (fd < 0)
	{
		std::cerr << "Error: send" << std::endl;
		return ;
	}
    std::string response = createResponse(result, fd);
	// デバッグ用メッセージ
    std::cerr << "Message: " << response;
	sendResponse(response, fd);
}
