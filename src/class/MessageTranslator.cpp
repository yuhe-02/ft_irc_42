#include "MessageTranslator.h"

MessageTranslator::MessageTranslator()
{
}

MessageTranslator::MessageTranslator(std::string pass) : pass_(pass)
{
	operator_pass_ = "admin";
	sender_ = Sender();
	channel_ = Channel::GetInstance();
	user_ = Everyone::GetInstance();
	func_["UNKNOWN"]   = &MessageTranslator::Unknown;
	func_["PASS"]      = &MessageTranslator::Pass;
	func_["NICK"]      = &MessageTranslator::Nick;
	func_["USER"]      = &MessageTranslator::User;
	func_["JOIN"]      = &MessageTranslator::Join;
	func_["PART"]      = &MessageTranslator::Part;
	func_["MODE"]      = &MessageTranslator::Mode;
	func_["INVITE"]    = &MessageTranslator::Invite;
	func_["KICK"]      = &MessageTranslator::Kick;
	func_["QUIT"]      = &MessageTranslator::Quit;
	func_["EXIT"]      = &MessageTranslator::Exit;
	func_["LOG"]       = &MessageTranslator::Log;
	func_["WHOIS"]       = &MessageTranslator::Whois;
	func_["PONG"]      = &MessageTranslator::Pong;
	func_["PING"]      = &MessageTranslator::Ping;
	tester_ = 1;
	// func_["SERVER"]    = &MessageTranslator::Server;
	// func_["OPER"]      = &MessageTranslator::Oper;
	// func_["SQUIT"]     = &MessageTranslator::Squit;
	// func_["NAMES"]     = &MessageTranslator::Names;
	// func_["LIST"]      = &MessageTranslator::List;
	// func_["VERSION"]   = &MessageTranslator::Version;
	// func_["STATS"]     = &MessageTranslator::Stats;
	// func_["LINKS"]     = &MessageTranslator::Links;
	// func_["TIME"]      = &MessageTranslator::Time;
	// func_["CONNECT"]   = &MessageTranslator::Connect;
	// func_["TRACE"]     = &MessageTranslator::Trace;
	// func_["ADMIN"]     = &MessageTranslator::Admin;
	// func_["INFO"]      = &MessageTranslator::Info;
	// func_["PRIVMSG"]   = &MessageTranslator::Privmsg;
	// func_["NOTICE"]    = &MessageTranslator::Notice;
	// func_["WHO"]       = &MessageTranslator::Who;
	// func_["WHOIS"]     = &MessageTranslator::Whois;
	// func_["WHOWAS"]    = &MessageTranslator::Whowas;
	// func_["KILL"]      = &MessageTranslator::Kill;
	// func_["PING"]      = &MessageTranslator::Ping;
	// func_["PONG"]      = &MessageTranslator::Pong;
	// func_["ERROR"]     = &MessageTranslator::Error;
	// func_["AWAY"]      = &MessageTranslator::Away;
	// func_["REHASH"]    = &MessageTranslator::Rehash;
	// func_["RESTART"]   = &MessageTranslator::Restart;
	// func_["SUMMON"]    = &MessageTranslator::Summon;
	// func_["USERS"]     = &MessageTranslator::Users;
	// func_["WALLOPS"]   = &MessageTranslator::Wallops;
	// func_["USERHOST"]  = &MessageTranslator::Userhost;
}

MessageTranslator::MessageTranslator(const MessageTranslator &)
{

}

MessageTranslator& MessageTranslator::operator=(const MessageTranslator&)
{
	return (*this);
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
bool  MessageTranslator::hasCommand(std::string str)
{
	if (func_.find(str) == func_.end())
	{
		std::cout << "matched: " << str << std::endl;
		return false;
	}
	return true;
}

void	MessageTranslator::Execute(std::string message, int user_fd)
{
	if (message == "")
		return ;
	std::vector<std::string> box;
	box = Translate(message);
	if (!user_->IsCreated(user_fd))
	{
		user_->CreateUser(user_fd);
		std::string tester("default");
		for (int n = 0; n < tester_; n++)
			tester += "0";
		// user_->SetNickname(user_fd, tester);
		// user_->SetUser(user_fd, "admin", "admin", "admin", "admin");
		tester_++;
	}
	if (box[0] == "ASKIP")
	{
		std::string tester("admin");
		for (int n = 0; n < tester_; n++)
			tester += "0";
		user_->SetRegister(user_fd, 1);
		user_->SetNickname(user_fd, tester);
		user_->SetUser(user_fd, "admin", "admin", "admin", "admin");
		std::cout << "ADMIN is comming" << std::endl;
		tester_++;
		sender_.SendMessage(create_code_message(1), user_fd);
		return ;
	}
	else if (box[0] == "SKIP")
	{
		std::string tester("user");
		for (int n = 0; n < tester_; n++)
			tester += "0";
		user_->SetRegister(user_fd, 1);
		user_->SetNickname(user_fd, tester);
		user_->SetUser(user_fd, "user", "user", "user", "user");
		std::cout << "USER is comming" << std::endl;
		tester_++;
		sender_.SendMessage(create_code_message(1), user_fd);
		return ;
	}
	if (!user_->IsRegisterAll(user_fd)) {
		if (!hasCommand(box[0])) {
			return ;
		} else if (box[0] == "PASS") {
			this->Pass(box, user_fd);
		} else if (box[0] == "NICK") {
			this->Nick(box, user_fd);
		} else if (box[0] == "USER") {
			this->User(box, user_fd);
		} else {
			sender_.SendMessage(ChannelResult(451, ""), user_fd);
		}
		return ;
	}
	if (box[0] == "PRIVMSG")
	{
		Privmsg(box, user_fd, message);
	} else if (box[0] == "TOPIC")
	{
		Topic(box, user_fd, message);
	} else if (!hasCommand(box[0]))
	{
		return ((this->*(func_["UNKNOWN"]))(box, user_fd));
	} else
	{
		(this->*(func_[box[0]]))(box, user_fd);
	}
	#ifdef DEBUG
		OutputLog();
	#endif
	return ;
}

void MessageTranslator::SetOpePass(std::string pass)
{
	operator_pass_ = pass;
}

void MessageTranslator::Ping(std::vector<std::string> av, int player_fd)
{
	std::string server2 = "";
	std::string pongMessage;

	if (av.size() < 2)
	{
		sender_.SendMessage(create_code_message(ERR_NOORIGIN, "PING"), player_fd);
		return ;
	}
	server2 = av[1];
	std::string serverName = "ft_irc";
	pongMessage = ":" + serverName + " PONG " + serverName + " :" + server2;
	sender_.SendMessage(ChannelResult(-1, pongMessage), player_fd);
}
void MessageTranslator::Pong(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 2)
	{
		sender_.SendMessage(create_code_message(ERR_NOORIGIN, "PONG"), player_fd);
		return ;
	}
	// sender_.SendMessage(ChannelResult(-1, "PONG"), player_fd);
}

void	MessageTranslator::Unknown(std::vector<std::string> av, int player_fd)
{
	// if (user_->IsCreated(player_fd))
	// 	sender_.SendMessage(create_code_message(ERR_UNKNOWNCOMMAND, av[0]), player_fd);
	// CAP LS時点ではユーザー作成されていないがunknownコマンドが来るので、コメントアウト
	sender_.SendMessage(create_code_message(ERR_UNKNOWNCOMMAND, av[0]), player_fd);
}

void	MessageTranslator::Pass(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 2)
	{
		sender_.SendMessage(create_code_message(ERR_NEEDMOREPARAMS, "PASS"), player_fd);
		return ;
	}
	if (user_->IsRegister(player_fd))
	{
		sender_.SendMessage(create_code_message(462, ""), player_fd);
		return ;
	}
	if (av[1] == pass_)
	{
		user_->SetRegister(player_fd, 1);
		return ;
	}
	if (av[1] == operator_pass_)
	{
		user_->SetRegister(player_fd, 1);
		return ;
	}
	sender_.SendMessage(create_code_message(ERR_PASSWDMISMATCH), player_fd);
}

void	MessageTranslator::Nick(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 2)
	{
		sender_.SendMessage(create_code_message(ERR_NONICKNAMEGIVEN, "NICK"), player_fd);
		return ;
	}
	ChannelResult result = user_->SetNickname(player_fd, av[1]);
	if (result.first == RPL_NOSEND)
	{
		return ;
	}
	sender_.SendMessage(result, player_fd);
	return ;
}

void	MessageTranslator::User(std::vector<std::string> av, int player_fd)
{
	// すでに登録が済んでいる場合はsyntax errorとかのバリデーション通さずに返す
	if (user_->IsRegisterUser(player_fd))
	{
		sender_.SendMessage(create_code_message(ERR_ALREADYREGISTRED), player_fd);
		return ;
	}
	if (av.size() != 5)
	{
		sender_.SendMessage(create_code_message(ERR_NEEDMOREPARAMS, "USER"), player_fd);
		return ;
	}
	// TODO 無効な文字がオプションに入ってきた時の対策
	ChannelResult result = user_->SetUser(player_fd, av[1], av[2], av[3], av[4]);
	if (result.first == RPL_NOSEND)
	{
		return ;
	}
	sender_.SendMessage(result, player_fd);
	return ;
}

void	MessageTranslator::Join(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 2)
	{
		sender_.SendMessage(create_code_message(ERR_NEEDMOREPARAMS, "JOIN"), player_fd);
		return ;
	}
	if (user_->IsRegister(player_fd) == false)
	{
		sender_.SendMessage(create_code_message(ERR_NOTREGISTERED), player_fd);
		return ;
	}
	std::stringstream ss(av[1]);
	std::string tmp;
	ChannelResult result;

	while (std::getline(ss, tmp, ','))
	{
		if (av.size() == 2)
		{
			result = channel_->JoinedChannel(player_fd, tmp);
		} else
		{
			result = channel_->JoinedChannel(player_fd, tmp);
		}
		sender_.SendMessage(result, player_fd);
		std::string str = ":" + user_->GetSomeone(player_fd).nick_name.back() + " JOIN :" + tmp;
		for (std::set<int>::iterator it = channel_->GetChannelInfo(tmp).joined_player.begin(); it != channel_->GetChannelInfo(tmp).joined_player.end(); it++)
		{
			sender_.SendMessage(ChannelResult(-1, str), *it);
		}
	}
}

void	MessageTranslator::Part(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 2)
	{
		sender_.SendMessage(create_code_message(ERR_NEEDMOREPARAMS, "PART"), player_fd);
		return ;
	}

	std::stringstream ss(av[1]);
	std::string tmp;

	if (av.size() == 2)
	{
		while (std::getline(ss, tmp, ','))
			sender_.SendMessage(channel_->LeaveChannel(player_fd, tmp), player_fd);
	}
	else
	{
		while (std::getline(ss, tmp, ','))
			sender_.SendMessage(channel_->LeaveChannel(player_fd, tmp, av[2]), player_fd);
	}
}

void MessageTranslator::Privmsg(std::vector<std::string> av, int player_fd, std::string str)
{
	if (av.size() == 1)
	{
		sender_.SendMessage(create_code_message(ERR_NORECIPIENT, "PRIVMSG"), player_fd);
		return ;
	}
	if (av.size() == 2)
	{
		sender_.SendMessage(create_code_message(ERR_NOTEXTTOSEND), player_fd);
		return ;
	}

	std::string tmp2 = str.substr(str.find(' ', str.find(' ') + 1) + 1);
	IntrusivePtr<Everyone> eve = Everyone::GetInstance();
	std::stringstream ss(av[1]);
	std::string tmp;
	ChannelResult result;
	while (std::getline(ss, tmp, ','))
	{
		result = channel_->SendMessageToChannel(player_fd, tmp, tmp2, sender_);
		if (result.first != -1)
			sender_.SendMessage(result, player_fd);
	}
}

void	MessageTranslator::Mode(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 2)
	{
		sender_.SendMessage(create_code_message(ERR_NEEDMOREPARAMS, "MODE"), player_fd);
		return ;
	}
	if (av.size() == 2)
		return ;
	const char *tmp = av[2].c_str() + 1;
	if (av.size() == 3)
	{
		if (av[2] == "+i")
			return ;
		sender_.SendMessage(channel_->ChangeChannelMode(player_fd, tmp, av[2][0] == '+', av[1]), player_fd);
		return ;
	}
	sender_.SendMessage(channel_->ChangeChannelMode(player_fd, tmp, av[2][0] == '+', av[1], av[3]), player_fd);
}

void	MessageTranslator::Topic(std::vector<std::string> av, int player_fd, std::string str)
{
	if (av.size() < 2)
	{
		sender_.SendMessage(create_code_message(ERR_NEEDMOREPARAMS, "TOPIC"), player_fd);
		return ;
	}
	if (av.size() == 2)
	{
		sender_.SendMessage(channel_->GetTopic(av[1]), player_fd);
		return ;
	}
	std::string tmp2 = str.substr(str.find(' ', str.find(' ') + 1) + 1);
	ChannelResult tmp = channel_->ChangeTopic(player_fd, av[1], tmp2);
	if (tmp.first == 1)
	{
		for (std::set<int>::iterator it = channel_->GetChannelInfo(av[1]).joined_player.begin(); it != channel_->GetChannelInfo(av[1]).joined_player.end(); it++)
			sender_.SendMessage(channel_->GetTopic(av[1]), *it);

	}
	else
		sender_.SendMessage(tmp, player_fd);
}

void MessageTranslator::Whois(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 2)
	{
		sender_.SendMessage(create_code_message(ERR_NONICKNAMEGIVEN), player_fd);
		return ;
	}
	if (av.size() > 2)
	{
		sender_.SendMessage(create_code_message(ERR_NOSUCHNICK), player_fd);
		sender_.SendMessage(create_code_message(RPL_ENDOFWHOIS), player_fd);
		return ;
	}
	user_->SendLog(av[1], player_fd);
}

void	MessageTranslator::Invite(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 3)
	{
		sender_.SendMessage(create_code_message(ERR_NEEDMOREPARAMS, "INVITE"), player_fd);
		return ;
	}
	ChannelResult tmp = channel_->InviteToChannel(player_fd, av[1], av[2]);
	if (tmp.first != RPL_INVITING)
	{
		sender_.SendMessage(tmp, player_fd);
		return ;
	}
	sender_.SendMessage(tmp, player_fd);
	sender_.SendMessage(ChannelResult(-1, user_->GetSomeone(player_fd).nick_name.back() + " INVITE " + av[1] + " :" + av[2]), user_->GetUserIdNick(av[1]));
	if (channel_->GetChannelInfo(av[2]).topic != "")
	{
		sender_.SendMessage(create_code_message(RPL_TOPIC, av[2], channel_->GetChannelInfo(av[2]).topic), player_fd);
		return ;
	}
	sender_.SendMessage(create_code_message(RPL_NOTOPIC, av[2]), player_fd);
	Sender sender;
	std::string str = ":" + av[1] + " JOIN :" + av[2];
	for (std::set<int>::iterator it = channel_->GetChannelInfo(av[2]).joined_player.begin(); it != channel_->GetChannelInfo(av[2]).joined_player.end(); it++)
		sender.SendMessage(ChannelResult(-1, str), *it);
}

void	MessageTranslator::Kick(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 3)
	{
		sender_.SendMessage(create_code_message(ERR_NEEDMOREPARAMS, "KICK"), player_fd);
		return ;
	}
	ChannelResult tmp;
	if (av.size() == 3)
		tmp = channel_->KickChannel(player_fd, av[2], av[1]);
	else
		tmp = channel_->KickChannel(player_fd, av[2], av[1], av[3]);
	if (tmp.first != 1)
		sender_.SendMessage(tmp, player_fd);
}

void	MessageTranslator::Quit(std::vector<std::string>, int player_fd)
{
	sender_.SendMessage(user_->DeleteUser(player_fd), player_fd);
}

void MessageTranslator::OutputLog()
{
	std::cout << "------------------------------------LOG----------------------------------------" << std::endl << std::endl;
	user_->OutputLog();
	channel_->OutputLog();
	std::cout << "----------------------------------ENDLOG---------------------------------------" << std::endl << std::endl;
}

void	MessageTranslator::Exit(std::vector<std::string>, int player_fd)
{
	if (!Everyone::GetInstance()->IsCreated(player_fd))
	{
		sender_.SendMessage(ChannelResult(FATAL, ""), player_fd);
		return ;
	}
	if (Everyone::GetInstance()->GetSomeone(player_fd).is_admin)
	{
		sender_.SendMessage(ChannelResult(-1, ""), player_fd);
		exit(0);
	}
	sender_.SendMessage(ChannelResult(FATAL, ""), player_fd);
}

void MessageTranslator::Log(std::vector<std::string>, int player_fd)
{
	if (!Everyone::GetInstance()->IsCreated(player_fd))
	{
		sender_.SendMessage(ChannelResult(FATAL, ""), player_fd);
		return ;
	}
	if (Everyone::GetInstance()->GetSomeone(player_fd).is_admin)
	{
		sender_.SendMessage(ChannelResult(-1, ""), player_fd);
		OutputLog();
		return ;
	}
	sender_.SendMessage(ChannelResult(FATAL, ""), player_fd);
}
