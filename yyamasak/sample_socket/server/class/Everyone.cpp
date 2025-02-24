#include "../includes/Everyone.h"

IntrusivePtr<Everyone> Everyone::instance_ = IntrusivePtr<Everyone>(0);

Everyone::Everyone() : RefCounted(), latest_user_id_(0)
{
	everyone_itos_.clear();
	everyone_.clear();
}

Everyone::Everyone(const Everyone &) : RefCounted()
{

}

Everyone &Everyone::operator=(const Everyone &)
{
	return (*this);
}

Everyone::~Everyone()
{

}

IntrusivePtr<Everyone>	Everyone::GetInstance()
{
	if (!instance_) {
		instance_ = IntrusivePtr<Everyone>(new Everyone());
	}
	return instance_;
}

pair<int, std::string>	Everyone::CreateUser(const std::string &username, const std::string &password, const std::string &hostname, const std::string &nickname, const std::string &realname)
{
	if (latest_user_id_ >= INT_MAX)
		return (std::pair<int, std::string>(FATAL, ""));
	if (password == "")
		return (std::pair<int, std::string>(FATAL, ""));

	Someone tmp;
	int user_id = static_cast<int>(latest_user_id_);

	tmp.block_user.clear();
	tmp.join_channel.clear();
	tmp.player_id = user_id;
	tmp.user_name = username;
	tmp.host_name = hostname;
	tmp.real_name = realname;
	tmp.password = password;
	if (nickname != "")
		tmp.nick_name.push_back(nickname);
	else
	{
		std::stringstream	str;
		str << user_id;
		tmp.nick_name.push_back(std::string("Guest") + str.str());
	}
	everyone_itos_[user_id] = username;
	everyone_[username] = tmp;
	latest_user_id_++;
	return (pair<int, std::string>(1, ""));
}

pair<int, std::string>	Everyone::DeleteUser(const std::string &player_str)
{
	if (ExistUser(player_str) <= 0)
		return (std::pair<int, std::string>(FATAL, ""));

	const Someone			tmp = everyone_[player_str];
	IntrusivePtr<Channel>	channel = Channel::GetInstance();

	for (std::set<std::string>::iterator i = tmp.join_channel.begin(); i != tmp.join_channel.end(); i++)
		channel->LeaveChannel(player_str, *i);
	channel->release();
	nick_list.erase(everyone_[player_str].nick_name.back());
	everyone_itos_.erase(tmp.player_id);
	everyone_.erase(player_str);
	return (pair<int, std::string>(1, ""));
}

pair<int, std::string>	Everyone::GetSomeone(const std::string &player_str, Someone &dest) const
{
	std::map<std::string, Someone>::const_iterator it = everyone_.find(player_str);
	if (it == everyone_.end())
		return (create_code_message(ERR_WASNOSUCHNICK, player_str));

	dest = it->second;
	return (pair<int, std::string>(1, ""));
}

pair<int, std::string>	Everyone::AddBlockUser(const std::string &player_str, const std::string& focas)
{
	if (!ExistUser(player_str))
		return (create_code_message(ERR_WASNOSUCHNICK, player_str));
	if (!ExistUser(focas))
		return (create_code_message(ERR_WASNOSUCHNICK, focas));
	everyone_[player_str].block_user.insert(focas);
	return (pair<int, std::string>(1, ""));
}

pair<int, std::string> Everyone::AddJoinChannel(const std::string &player_str, const std::string& focas)
{
	if (!ExistUser(player_str))
		return (create_code_message(ERR_WASNOSUCHNICK, player_str));
	everyone_[player_str].join_channel.insert(focas);
	return (pair<int, std::string>(1, ""));
}

pair<int, std::string> Everyone::DeleteJoinChannel(const std::string &player_str, const std::string& focas)
{
	if (!ExistUser(player_str))
		return (create_code_message(ERR_WASNOSUCHNICK, player_str));
	everyone_[player_str].join_channel.erase(focas);
	return (pair<int, std::string>(1, ""));
}

pair<int, std::string>	Everyone::DeleteBlockUser(const std::string &player_str, const std::string& focas)
{
	if (!ExistUser(player_str))
		return (create_code_message(ERR_WASNOSUCHNICK, player_str));
	if (!ExistUser(player_str))
		return (create_code_message(ERR_WASNOSUCHNICK, focas));

	everyone_[player_str].block_user.erase(focas);
	return (pair<int, std::string>(1, ""));
}

pair<int, std::string> Everyone::SetUsername(const std::string &player_str, const std::string &username)
{
	if (!ExistUser(player_str))
		return (create_code_message(ERR_WASNOSUCHNICK, player_str));
	everyone_[player_str].user_name = username;
	return (pair<int, std::string>(1, ""));
}

pair<int, std::string>	Everyone::SetHostname(const std::string &player_str, const std::string &hostname)
{
	if (!ExistUser(player_str))
		return (create_code_message(ERR_WASNOSUCHNICK, player_str));
	everyone_[player_str].host_name = hostname;
	return (pair<int, std::string>(1, ""));
}

pair<int, std::string>	Everyone::SetRealname(const std::string &player_str, const std::string &realname)
{
	if (!ExistUser(player_str))
		return (create_code_message(ERR_WASNOSUCHNICK, player_str));
	if (realname == "")
		return (std::pair<int, std::string>(FATAL, ""));

	everyone_[player_str].real_name = realname;
	return (pair<int, std::string>(1, ""));
}

pair<int, std::string>	Everyone::SetNickname(const std::string &player_str, const std::string &nickname)
{
	if (!ExistUser(player_str))
		return (create_code_message(ERR_WASNOSUCHNICK, player_str));
	if (nickname == "")
		return (std::pair<int, std::string>(FATAL, ""));
	if (nick_list.find(nickname) == nick_list.end())
		return (create_code_message(ERR_NICKNAMEINUSE, nickname));
	nick_list.erase(everyone_[player_str].nick_name.back());
	everyone_[player_str].nick_name.push_back(nickname);
	nick_list.insert(nickname);
	return (pair<int, std::string>(1, ""));
}

pair<int, std::string>	Everyone::SetPassword(const std::string &player_str, const std::string &password)
{
	if (!ExistUser(player_str))
		return (create_code_message(ERR_WASNOSUCHNICK, player_str));
	everyone_[player_str].password = password;
	return (pair<int, std::string>(1, ""));
}

bool	Everyone::ExistUser(const std::string &player_str) const
{
	std::map<std::string, Someone>::const_iterator it = everyone_.find(player_str);
	if (it == everyone_.end())
		return (false);
	return (true);
}
