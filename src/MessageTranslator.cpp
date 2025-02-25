#include "MessageTranslator.h"

MessageTranslator::MessageTranslator()
{
	func_["UNKNOWN"] = &Unknown;
	func_["PASS"] = &Pass;
	func_["NICK"] = &Nick;
	func_["USER"] = &User;
}

MessageTranslator::MessageTranslator(const MessageTranslator& other)
{

}

MessageTranslator& MessageTranslator::operator=(const MessageTranslator& other)
{

}

MessageTranslator::~MessageTranslator()
{

}

std::vector<std::string> MessageTranslator::Translate(std::string str)
{
	std::vector<std::string>	box;
	std::stringstream			ss(str);
	std::string					tmp;

	while (ss >> tmp)
		box.push_back(tmp);
	return (box);
}

void	MessageTranslator::Execute(std::string message, int user_fd)
{
	std::vector<std::string> box;

	box = Translate(message);
	if (func_.find(box[0]) == func_.end())
	{
		(this->*(func_["UNKNOWN"]))(box);
		return ;
	}
	(this->*(func_[box[0]]))(box);
}

ChannelResult MessageTranslator::Unknown(std::vector<std::string> av)
{
	return (ChannelResult(1, ""));
}

ChannelResult MessageTranslator::Pass(std::vector<std::string> av)
{
	if (av.size() == 1)
		return (ChannelResult(ERR_NEEDMOREPARAMS, ""));
	return (ChannelResult(1, ""));
}

ChannelResult MessageTranslator::Nick(std::vector<std::string> av)
{
	return (ChannelResult(1, ""));
}

ChannelResult MessageTranslator::User(std::vector<std::string> av)
{
	return (ChannelResult(1, ""));
}
