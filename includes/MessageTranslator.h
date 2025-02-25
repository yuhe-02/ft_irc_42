#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <utility>

#include "ErrorCode.h"

class MessageTranslator
{
private:
	std::string pass_;
	std::map<std::string, ChannelResult (MessageTranslator::*)(std::vector<std::string>)> func_;

	std::vector<std::string>	Translate(std::string str);
	ChannelResult	Unknown(std::vector<std::string> av);
	ChannelResult	Pass(std::vector<std::string> av);
	ChannelResult	Nick(std::vector<std::string> av);
	ChannelResult	User(std::vector<std::string> av);

public:
	MessageTranslator();
	MessageTranslator(const MessageTranslator& other);
	MessageTranslator& operator=(const MessageTranslator& other);
	~MessageTranslator();

	void	Execute(std::string message, int user_fd);
};
