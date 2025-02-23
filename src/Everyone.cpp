#include "../includes/Everyone.h"
#include "Everyone.h"

IntrusivePtr<Everyone> Everyone::instance_ = IntrusivePtr<Everyone>(static_cast<Everyone*>(NULL));

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

ChannelResult	Everyone::CreateUser(const std::string &username, const std::string &password, const std::string &hostname, const std::string &nickname, const std::string &realname)
{
	if (latest_user_id_ >= INT_MAX)
		return (ChannelResult(FATAL, ""));
	if (password == "")
		return (ChannelResult(FATAL, ""));
	if (nick_list.find(nickname) != nick_list.end())
		return (ChannelResult(ERR_NICKNAMEINUSE, nickname));

	Someone tmp;
	int user_id = static_cast<int>(latest_user_id_);

	tmp.player_id = user_id;
	tmp.user_name = username;
	tmp.host_name = hostname;
	tmp.real_name = realname;
	tmp.password = password;
	if (nickname != "")
	{
		tmp.nick_name.push_back(nickname);
		nick_list.insert(nickname);
	}
	else
	{
		std::stringstream	str;
		str << user_id;
		tmp.nick_name.push_back(std::string("Guest") + str.str());
		nick_list.insert(std::string("Guest") + str.str());
	}
	everyone_itos_[user_id] = username;
	everyone_[username] = tmp;
	latest_user_id_++;
	return (ChannelResult(1, ""));
}

ChannelResult	Everyone::DeleteUser(const std::string &player_str)
{
	if (ExistUser(player_str) <= 0)
		return (ChannelResult(FATAL, ""));

	const Someone			tmp = everyone_[player_str];
	IntrusivePtr<Channel>	channel = Channel::GetInstance();

	for (std::set<std::string>::iterator i = tmp.join_channel.begin(); i != tmp.join_channel.end(); i++)
		channel->LeaveChannel(player_str, *i);
	nick_list.erase(everyone_[player_str].nick_name.back());
	everyone_itos_.erase(tmp.player_id);
	everyone_.erase(player_str);
	return (ChannelResult(1, ""));
}

ChannelResult	Everyone::GetSomeone(const std::string &player_str, Someone &dest) const
{
	std::map<std::string, Someone>::const_iterator it = everyone_.find(player_str);
	if (it == everyone_.end())
		return (create_code_message(ERR_WASNOSUCHNICK, player_str));

	dest = it->second;
	return (ChannelResult(1, ""));
}

ChannelResult	Everyone::AddBlockUser(const std::string &player_str, const std::string& focas)
{
	if (!ExistUser(player_str))
		return (create_code_message(ERR_WASNOSUCHNICK, player_str));
	if (!ExistUser(focas))
		return (create_code_message(ERR_WASNOSUCHNICK, focas));
	everyone_[player_str].block_user.insert(focas);
	return (ChannelResult(1, ""));
}

ChannelResult Everyone::AddJoinChannel(const std::string &player_str, const std::string& focas)
{
	if (!ExistUser(player_str))
		return (create_code_message(ERR_WASNOSUCHNICK, player_str));
	everyone_[player_str].join_channel.insert(focas);
	return (ChannelResult(1, ""));
}

ChannelResult Everyone::DeleteJoinChannel(const std::string &player_str, const std::string& focas)
{
	if (!ExistUser(player_str))
		return (create_code_message(ERR_WASNOSUCHNICK, player_str));
	everyone_[player_str].join_channel.erase(focas);
	return (ChannelResult(1, ""));
}

ChannelResult	Everyone::DeleteBlockUser(const std::string &player_str, const std::string& focas)
{
	if (!ExistUser(player_str))
		return (create_code_message(ERR_WASNOSUCHNICK, player_str));
	if (!ExistUser(focas))
		return (create_code_message(ERR_WASNOSUCHNICK, focas));

	everyone_[player_str].block_user.erase(focas);
	return (ChannelResult(1, ""));
}

ChannelResult Everyone::SetUsername(const std::string &player_str, const std::string &username)
{
	if (!ExistUser(player_str))
		return (create_code_message(ERR_WASNOSUCHNICK, player_str));
	everyone_[player_str].user_name = username;
	return (ChannelResult(1, ""));
}

ChannelResult	Everyone::SetHostname(const std::string &player_str, const std::string &hostname)
{
	if (!ExistUser(player_str))
		return (create_code_message(ERR_WASNOSUCHNICK, player_str));
	everyone_[player_str].host_name = hostname;
	return (ChannelResult(1, ""));
}

ChannelResult	Everyone::SetRealname(const std::string &player_str, const std::string &realname)
{
	if (!ExistUser(player_str))
		return (create_code_message(ERR_WASNOSUCHNICK, player_str));
	if (realname == "")
		return (ChannelResult(FATAL, ""));

	everyone_[player_str].real_name = realname;
	return (ChannelResult(1, ""));
}

ChannelResult	Everyone::SetNickname(const std::string &player_str, const std::string &nickname)
{
	if (!ExistUser(player_str))
		return (create_code_message(ERR_WASNOSUCHNICK, player_str));
	if (nickname == "")
		return (ChannelResult(FATAL, ""));
	if (nick_list.find(nickname) != nick_list.end())
		return (create_code_message(ERR_NICKNAMEINUSE, nickname));
	nick_list.erase(everyone_[player_str].nick_name.back());
	everyone_[player_str].nick_name.push_back(nickname);
	nick_list.insert(nickname);
	return (ChannelResult(1, ""));
}

ChannelResult	Everyone::SetPassword(const std::string &player_str, const std::string &password)
{
	if (!ExistUser(player_str))
		return (create_code_message(ERR_WASNOSUCHNICK, player_str));
	everyone_[player_str].password = password;
	return (ChannelResult(1, ""));
}

bool	Everyone::ExistUser(const std::string &player_str) const
{
	std::set<std::string>::const_iterator it = nick_list.find(player_str);
	if (it == nick_list.end())
		return (false);
	return (true);
}
