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

void	MessageTranslator::Execute(std::string message, int user_fd)
{
	if (message == "")
		return ;
	if (message == "ASKIP")
	{
		user_->CreateUser(user_fd, 1);
		std::string tester("admin");
		for (int n = 0; n < tester_; n++)
			tester += "0";
		user_->SetNickname(user_fd, tester);
		user_->SetUser(user_fd, "admin", "admin", "admin", "admin");
		std::cout << "ADMIN is comming" << std::endl;
		tester_++;
		return ;
	}
	if (message == "SKIP")
	{
		user_->CreateUser(user_fd);
		std::string tester("user");
		for (int n = 0; n < tester_; n++)
			tester += "0";
		user_->SetNickname(user_fd, tester);
		user_->SetUser(user_fd, "user", "user", "user", "user");
		std::cout << "USER is comming" << std::endl;
		tester_++;
		return ;
	}
	std::vector<std::string> box;
	box = Translate(message);
	if (box[0] == "PONG")
		return ;
	if (box[0] == "PING")
	{
		sender_.SendMessage(ChannelResult(1, "PONG"), user_fd);
		return ;
	}
	if (!box.size() || (func_.find(box[0]) == func_.end() && box[0] != "TOPIC" && box[0] != "PRIVMSG" && box[0] != "CAP"))
		return ((this->*(func_["UNKNOWN"]))(box, user_fd));
	if (box[0] == "CAP")
	{
		if (box[1] != "END")
			sender_.SendMessage(ChannelResult(1, "CAP * LS :"), user_fd);
		return ;
	}
	if (box[0] == "PASS")
	{
		Pass(box, user_fd);
		return ;
	}
	if (!user_->IsCreated(user_fd))
	{
		sender_.SendMessage(create_code_message(ERR_NOTREGISTERED), user_fd);
		return ;
	}
	if (box[0] == "PRIVMSG")
		Privmsg(box, user_fd, message);
	else if (box[0] == "TOPIC")
		Topic(box, user_fd, message);
	else
		(this->*(func_[box[0]]))(box, user_fd);
	#ifdef DEBUG
		OutputLog();
	#endif
	return ;
}

void MessageTranslator::SetOpePass(std::string pass)
{
	operator_pass_ = pass;
}

void	MessageTranslator::Unknown(std::vector<std::string> av, int player_fd)
{
	sender_.SendMessage(create_code_message(ERR_UNKNOWNCOMMAND, av[0]), player_fd);
}

void	MessageTranslator::Pass(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 2)
	{
		sender_.SendMessage(create_code_message(ERR_NEEDMOREPARAMS, "PASS"), player_fd);
		return ;
	}
	if (av[1] == pass_)
	{
		sender_.SendMessage(user_->CreateUser(player_fd), player_fd);
		return ;
	}
	if (av[1] == operator_pass_)
	{
		sender_.SendMessage(user_->CreateUser(player_fd, 1), player_fd);
		return ;
	}
	sender_.SendMessage(ChannelResult(FATAL, ""), player_fd);
}

void	MessageTranslator::Nick(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 2)
	{
		sender_.SendMessage(create_code_message(ERR_NONICKNAMEGIVEN, "NICK"), player_fd);
		return ;
	}
	sender_.SendMessage(user_->SetNickname(player_fd, av[1]), player_fd);
}

void	MessageTranslator::User(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 5)
	{
		sender_.SendMessage(create_code_message(ERR_NEEDMOREPARAMS, "USER"), player_fd);
		return ;
	}
	sender_.SendMessage(user_->SetUser(player_fd, av[1], av[2], av[3], av[4]), player_fd);
}

void	MessageTranslator::Join(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 2)
	{
		sender_.SendMessage(create_code_message(ERR_NEEDMOREPARAMS, "JOIN"), player_fd);
		return ;
	}

	std::stringstream ss(av[1]);
	std::string tmp;

	while (std::getline(ss, tmp, ','))
	{
		if (av.size() == 2)
			sender_.SendMessage(channel_->JoinedChannel(player_fd, tmp), player_fd);
		else
			sender_.SendMessage(channel_->JoinedChannel(player_fd, tmp, 0, av[2]), player_fd);
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

	while (std::getline(ss, tmp, ','))
		sender_.SendMessage(channel_->LeaveChannel(player_fd, tmp), player_fd);
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
		if (result.first != RPL_AWAY)
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
	std::cout << tmp2 << std::endl;
	sender_.SendMessage(channel_->ChangeTopic(player_fd, av[1], tmp2), player_fd);
}

void MessageTranslator::Whois(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 2)
	{
		sender_.SendMessage(create_code_message(ERR_NONICKNAMEGIVEN), player_fd);
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
	sender_.SendMessage(channel_->InviteToChannel(player_fd, av[1], av[2]), player_fd);
}

void	MessageTranslator::Kick(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 3)
	{
		sender_.SendMessage(create_code_message(ERR_NEEDMOREPARAMS, "KICK"), player_fd);
		return ;
	}
	if (av.size() == 3)
	{
		sender_.SendMessage(channel_->KickChannel(player_fd, av[2], av[1]), player_fd);
		return ;
	}
	sender_.SendMessage(channel_->KickChannel(player_fd, av[2], av[1], av[3]), player_fd);
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
		sender_.SendMessage(ChannelResult(1, ""), player_fd);
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
		sender_.SendMessage(ChannelResult(1, ""), player_fd);
		OutputLog();
		return ;
	}
	sender_.SendMessage(ChannelResult(FATAL, ""), player_fd);
}

// 一応残してるけどいらないでしょこれ。
// /// @brief
// /// @param av
// /// @param player_fd リストを送っているときと、リストが終わった時でメッセージが違う。メッセージ送信がもろ関わるため保留
// /// @return
// ChannelResult	MessageTranslator::Names(std::vector<std::string> av, int player_fd) {
// 	return ChannelResult();
// }

// /// @brief
// /// @param av
// /// @param player_fd リストを送っているときと、リストが終わった時でメッセージが違う。メッセージ送信がもろ関わるため保留
// /// @return
// ChannelResult	MessageTranslator::List(std::vector<std::string> av, int player_fd) {
// 	return ChannelResult();
// }
// ChannelResult	MessageTranslator::Version(std::vector<std::string> av, int player_fd)
// {
	// 	return (create_code_message(RPL_VERSION, "version: 42.195"));
	// }

	// /// @brief
	// /// @param av
	// /// @param player_fd いるこれ？
	// /// @return
	// ChannelResult	MessageTranslator::Time(std::vector<std::string> av, int player_fd) {
		// 	return ChannelResult();
		// }

		// /// @brief
		// /// @param av
		// /// @param player_fd 送信系のため未対応
		// /// @return
		// ChannelResult	MessageTranslator::Privmsg(std::vector<std::string> av, int player_fd) {
			// }




// ChannelResult	MessageTranslator::Who(std::vector<std::string> av, int player_fd) {
	// 	return ChannelResult();
	// }
	// ChannelResult	MessageTranslator::Whois(std::vector<std::string> av, int player_fd) {
		// 	return ChannelResult();
		// }
		// ChannelResult	MessageTranslator::Whowas(std::vector<std::string> av, int player_fd) {
			// 	return ChannelResult();
			// }
			// ChannelResult	MessageTranslator::Kill(std::vector<std::string> av, int player_fd) {
				// 	return ChannelResult();
				// }
				// ChannelResult	MessageTranslator::Ping(std::vector<std::string> av, int player_fd) {
					// 	return ChannelResult();
					// }
// ChannelResult	MessageTranslator::Pong(std::vector<std::string> av, int player_fd) {
// 	return ChannelResult();
// }
// ChannelResult	MessageTranslator::Error(std::vector<std::string> av, int player_fd) {
// 	return ChannelResult();
// }
// ChannelResult	MessageTranslator::Away(std::vector<std::string> av, int player_fd) {
// 	return ChannelResult();
// }
// ChannelResult	MessageTranslator::Rehash(std::vector<std::string> av, int player_fd) {
// 	return ChannelResult();
// }
// ChannelResult	MessageTranslator::Restart(std::vector<std::string> av, int player_fd) {
// 	return ChannelResult();
// }
// ChannelResult	MessageTranslator::Summon(std::vector<std::string> av, int player_fd) {
// 	return ChannelResult();
// }
// ChannelResult	MessageTranslator::Users(std::vector<std::string> av, int player_fd) {
// 	return ChannelResult();
// }
// ChannelResult	MessageTranslator::Wallops(std::vector<std::string> av, int player_fd) {
// 	return ChannelResult();
// }
// ChannelResult	MessageTranslator::Userhost(std::vector<std::string> av, int player_fd) {
// 	return ChannelResult();
// }
