#include "Sender.hpp"

static std::string zfill(int num, int digit)
{
    std::stringstream ss;
    ss << num;
    std::string str = ss.str();

    while (str.length() < static_cast<size_t>(digit))
    {
        str = "0" + str;
    }
    return str;
}
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

void	Sender::SendMessage(ChannelResult result, int fd) const
{
	ssize_t		byte_send;
	std::string	message;

	if (fd < 0)
	{
		std::cerr << "Error: send" << std::endl;
		return ;
	}
	std::stringstream ss;
	if (result.first != -1)
	{
		// std::cerr << "ChannelResult: " << result.first << ", " << result.second << std::endl;//デバッグ用
		ss << ":localhost " << zfill(result.first, 3);
		if ((result.first != 451 && result.first != 464) && (result.second.find("PASS") == std::string::npos))
		{
			// unknown command時にニックネームがない可能性があるため
			if (Everyone::GetInstance()->IsCreated(fd))
			{
				ss << " " << Everyone::GetInstance()->GetSomeone(fd).nick_name.back();
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
	if (!ss.str().empty())
	{
		ss << "\r\n";
	}
	std::cerr << "Message: " << ss.str();//デバッグ用
	// message += "\r\n";
	byte_send = send(fd, ss.str().c_str(), ss.str().length(), 0);
	if (byte_send == -1)
	{
		std::cerr << "Error: send" << std::endl;
	}
}
