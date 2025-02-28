#include "MessageTranslator.h"

MessageTranslator::MessageTranslator()
{
}

MessageTranslator::MessageTranslator(std::string pass) : pass_(pass)
{
	operator_pass_ = "admin";
	channel_ = Channel::GetInstance();
	user_ = Everyone::GetInstance();
	func_["UNKNOWN"]   = &MessageTranslator::Unknown;
	func_["PASS"]      = &MessageTranslator::Pass;
	func_["NICK"]      = &MessageTranslator::Nick;
	func_["USER"]      = &MessageTranslator::User;
	func_["JOIN"]      = &MessageTranslator::Join;
	func_["PART"]      = &MessageTranslator::Part;
	func_["MODE"]      = &MessageTranslator::Mode;
	func_["TOPIC"]     = &MessageTranslator::Topic;
	func_["INVITE"]    = &MessageTranslator::Invite;
	func_["KICK"]      = &MessageTranslator::Kick;
	func_["QUIT"]      = &MessageTranslator::Quit;
	func_["EXIT"]      = &MessageTranslator::Exit;
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

ChannelResult	MessageTranslator::Execute(std::string message, int user_fd)
{
	if (message == "")
		return (ChannelResult(FATAL, ""));
	std::vector<std::string> box;
	box = Translate(message);
	std::cout <<box[0]<< std::endl;
	if (!box.size() || func_.find(box[0]) == func_.end())
		return ((this->*(func_["UNKNOWN"]))(box, user_fd));
	ChannelResult result = (this->*(func_[box[0]]))(box, user_fd);
	#ifdef DEBUG
		OutputLog();
	#endif
	return (result);
}

void MessageTranslator::SetOpePass(std::string pass)
{
	operator_pass_ = pass;
}

ChannelResult	MessageTranslator::Unknown(std::vector<std::string>, int)
{
	return (ChannelResult(FATAL, ""));
}

ChannelResult	MessageTranslator::Pass(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 2 || (av[1] != pass_ && av[1] != operator_pass_))
		return (create_code_message(ERR_NEEDMOREPARAMS, "PASS"));
	if (av[1] == pass_)
		return (user_->CreateUser(player_fd));
	if (av[1] == operator_pass_)
		return (user_->CreateUser(player_fd, 1));
	return (ChannelResult(FATAL, ""));
}

ChannelResult	MessageTranslator::Nick(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 2)
		return (create_code_message(ERR_NONICKNAMEGIVEN, "NICK"));
	return (user_->SetNickname(player_fd, av[1]));
}

ChannelResult	MessageTranslator::User(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 5)
		return (create_code_message(ERR_NEEDMOREPARAMS, "USER"));
	return (user_->SetUser(player_fd, av[1], av[2], av[3], av[4]));
}


/// @brief
/// @param av
/// @param player_fd 複数チャンネル選択できない、、、#または&で始まるからそれで識別するべき
/// @return
ChannelResult	MessageTranslator::Join(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 2)
		return (create_code_message(ERR_NEEDMOREPARAMS, "JOIN"));
	if (av.size() == 2)
		return (channel_->JoinedChannel(player_fd, av[1]));
	return (channel_->JoinedChannel(player_fd, av[1], 0, av[2]));
}

/// @brief
/// @param av
/// @param player_fd 複数チャンネル選択できない、、、#または&で始まるからそれで識別するべき
/// @return
ChannelResult	MessageTranslator::Part(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 2)
	return (create_code_message(ERR_NEEDMOREPARAMS, "PART"));
	return (channel_->LeaveChannel(player_fd, av[1]));
}

ChannelResult	MessageTranslator::Mode(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 3)
	return (create_code_message(ERR_NEEDMOREPARAMS, "MODE"));
	const char *tmp = av[2].c_str() + 1;
	if (av.size() == 3)
	return (channel_->ChangeChannelMode(player_fd, tmp, av[2][0] == '+', av[1]));
	return (channel_->ChangeChannelMode(player_fd, tmp, av[2][0] == '+', av[1], av[3]));
}

ChannelResult	MessageTranslator::Topic(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 2)
	return (create_code_message(ERR_NEEDMOREPARAMS, "TOPIC"));
	if (av.size() == 2)
	return (channel_->GetTopic(av[1]));
	return (channel_->ChangeTopic(player_fd, av[1], av[2]));
}


ChannelResult	MessageTranslator::Invite(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 3)
	return (create_code_message(ERR_NEEDMOREPARAMS, "INVITE"));
	return (channel_->InviteToChannel(player_fd, av[1], av[2]));
}

/// @brief
/// @param av
/// @param player_fd 第三引数コメント未実装, 複数ユーザー未実装
/// @return
ChannelResult	MessageTranslator::Kick(std::vector<std::string> av, int player_fd)
{
	if (av.size() < 3)
		return (create_code_message(ERR_NEEDMOREPARAMS, "KICK"));
	return (channel_->KickChannel(player_fd, av[2], av[1]));
}

ChannelResult	MessageTranslator::Quit(std::vector<std::string>, int player_fd)
{
	return (user_->DeleteUser(player_fd));
}

void MessageTranslator::OutputLog()
{
	std::cout << "------------------------------------LOG----------------------------------------" << std::endl << std::endl;
	user_->OutputLog();
	channel_->OutputLog();
	std::cout << "----------------------------------ENDLOG---------------------------------------" << std::endl << std::endl;
}

ChannelResult	MessageTranslator::Exit(std::vector<std::string>, int player_fd)
{
	if (Everyone::GetInstance()->GetSomeone(player_fd).is_admin)
		exit(0);
	return (ChannelResult(FATAL, ""));
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
