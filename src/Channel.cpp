#include "Channel.h"

IntrusivePtr<Channel> Channel::instance_ = IntrusivePtr<Channel>(static_cast<Channel*>(NULL));

Channel::Channel() : RefCounted()
{

}

Channel::Channel(const Channel &) : RefCounted()
{

}

Channel &Channel::operator=(const Channel &)
{
	return (*this);
}

Channel::~Channel()
{

}

IntrusivePtr<Channel>	Channel::GetInstance()
{
	if (!instance_) {
		instance_ = IntrusivePtr<Channel>(new Channel());
	}
	return instance_;
}

ChannelResult	Channel::CreateChannel(int player_fd, const std::string& name)
{
	IntrusivePtr<Everyone> eve = Everyone::GetInstance();
	if (!eve->IsRegister(player_fd))
		return (create_code_message(ERR_NOTREGISTERED));
	if (channels_.find(name) != channels_.end())
		return (ChannelResult(FATAL, ""));
	if (channels_.size() >= INT_MAX)
		return (ChannelResult(FATAL, ""));
	if (channels_.find(name) != channels_.end())
		return (ChannelResult(FATAL, ""));
	if (name.size() > 200)
		return (ChannelResult(FATAL, ""));
	if (name == "" || !(name[0] == '#' || name[0] == '&'))
		return (ChannelResult(FATAL, ""));
	if (name.find(' ') != std::string::npos)
		return (ChannelResult(FATAL, ""));
	if (name.find(',') != std::string::npos)
		return (ChannelResult(FATAL, ""));
	if (name.find(7) != std::string::npos)
		return (ChannelResult(FATAL, ""));

	ChannelInfo tmp;

	tmp.channel_name = name;
	tmp.is_master.insert(player_fd);
	tmp.is_invite = false;
	tmp.is_topic = false;
	tmp.is_key = false;
	tmp.is_limit = false;
	tmp.limit_member = 0;
	tmp.is_master.insert(player_fd);
	tmp.joined_player.insert(player_fd);
	channels_[name] = tmp;
	IntrusivePtr<Everyone> tmpe = Everyone::GetInstance();
	channels_[name].joined_player.insert(player_fd);
	tmpe->AddJoinChannel(player_fd, name);
	return (create_code_message(RPL_TOPIC, name, channels_[name].topic));
}

ChannelResult	Channel::DeleteChannel(const std::string& channel_str)
{
	channels_.erase(channel_str);
	return (ChannelResult(1, ""));
}

const ChannelInfo&	Channel::GetChannelInfo(const std::string& channel_str) const
{
	std::map<std::string, ChannelInfo>::const_iterator it = channels_.find(channel_str);
	return (it->second);
}

ChannelResult	Channel::InviteToChannel(int player_fd, const std::string &focas_user_str, const std::string& channel_str)
{
	IntrusivePtr<Everyone> eve = Everyone::GetInstance();
	if (!eve->IsRegister(player_fd))
		return (create_code_message(ERR_NOTREGISTERED));
	IntrusivePtr<Everyone> tmp = Everyone::GetInstance();
	if (!tmp->ExistUserNick(focas_user_str))
		return (create_code_message(ERR_NOSUCHNICK, focas_user_str));
	if (!IsJoined(player_fd, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if (IsJoined(tmp->GetUserIdNick(focas_user_str), channel_str))
		return (create_code_message(ERR_USERONCHANNEL, focas_user_str, channel_str));
	if (!IsOperator(player_fd, channel_str))
		return (create_code_message(ERR_CHANOPRIVSNEEDED, channel_str));
	if (channels_[channel_str].joined_player.size() == static_cast<size_t>(channels_[channel_str].limit_member))
		return (create_code_message(ERR_CHANNELISFULL, channel_str));
	JoinedChannel(tmp->GetUserIdNick(focas_user_str), channel_str, 1);
	return (create_code_message(RPL_AWAY, focas_user_str));
}

ChannelResult	Channel::JoinedChannel(int player_fd, const std::string& channel_str, int flag, std::string pass)
{
	IntrusivePtr<Everyone> tmp = Everyone::GetInstance();
	if (!tmp->IsRegister(player_fd))
		return (create_code_message(ERR_NOTREGISTERED));

	if (!ExistChannel(channel_str))
		return (CreateChannel(player_fd, channel_str));
	if (IsJoined(player_fd, channel_str))
		return (create_code_message(ERR_USERONCHANNEL, tmp->GetSomeone(player_fd).nick_name.back(), channel_str));
	if (!tmp->IsAdmin(player_fd))
	{
		if (flag == 0 && channels_[channel_str].is_invite)
			return (create_code_message(ERR_INVITEONLYCHAN, channel_str));
		if (channels_[channel_str].is_limit && channels_[channel_str].joined_player.size() == static_cast<size_t>(channels_[channel_str].limit_member))
			return (create_code_message(ERR_CHANNELISFULL, channel_str));
		if (tmp->GetSomeone(player_fd).join_channel.size() > 10)
			return (create_code_message(ERR_TOOMANYCHANNELS, channel_str));
		if (channels_[channel_str].is_key && channels_[channel_str].password != pass)
			return (create_code_message(ERR_BADCHANNELKEY, channel_str));
	}
	channels_[channel_str].joined_player.insert(player_fd);
	tmp->AddJoinChannel(player_fd, channel_str);
	return (create_code_message(RPL_TOPIC, channel_str, channels_[channel_str].topic));
}

ChannelResult	Channel::LeaveChannel(int player_fd, const std::string& channel_str)
{
	IntrusivePtr<Everyone> eve = Everyone::GetInstance();
	if (!eve->IsRegister(player_fd))
		return (create_code_message(ERR_NOTREGISTERED));
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (!IsJoined(player_fd, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	channels_[channel_str].joined_player.erase(player_fd);
	channels_[channel_str].is_master.erase(player_fd);
	IntrusivePtr<Everyone> tmp = Everyone::GetInstance();
	tmp->DeleteJoinChannel(player_fd, channel_str);
	if (channels_[channel_str].joined_player.size() == 0)
		DeleteChannel(channel_str);
	return (ChannelResult(1, ""));
}

ChannelResult	Channel::KickChannel(int player_fd, const std::string &focas_user_str, const std::string& channel_str, std::string message)
{
	IntrusivePtr<Everyone> eve = Everyone::GetInstance();
	if (!eve->ExistUserNick(focas_user_str))
		return (create_code_message(ERR_NOSUCHNICK, focas_user_str));
	if (!eve->IsRegister(player_fd))
		return (create_code_message(ERR_NOTREGISTERED));
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (!eve->IsAdmin(player_fd) && !IsJoined(player_fd, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if (!IsOperator(player_fd, channel_str))
		return (create_code_message(ERR_CHANOPRIVSNEEDED, channel_str));
	IntrusivePtr<Everyone> tmp = Everyone::GetInstance();
	LeaveChannel(tmp->GetUserIdNick(focas_user_str), channel_str);
	return (ChannelResult(1, message));
}

ChannelResult	Channel::ChangeTopic(int player_fd, const std::string& channel_str, std::string str)
{
	IntrusivePtr<Everyone> eve = Everyone::GetInstance();
	if (!eve->IsRegister(player_fd))
		return (create_code_message(ERR_NOTREGISTERED));
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if (!eve->IsAdmin(player_fd) && !IsJoined(player_fd, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if ((channels_[channel_str].is_topic) && !IsOperator(player_fd, channel_str))
		return (create_code_message(ERR_CHANOPRIVSNEEDED, channel_str));


	channels_[channel_str].topic = str;
	return (ChannelResult(1, ""));
}

ChannelResult	Channel::ChangeChannelMode(int player_fd, const std::string& mode,
		bool valid, const std::string& channel_str, std::string key)
{
	IntrusivePtr<Everyone> eve = Everyone::GetInstance();
	if (!eve->IsRegister(player_fd))
		return (create_code_message(ERR_NOTREGISTERED));
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (!eve->IsAdmin(player_fd) && !IsJoined(player_fd, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if (!IsOperator(player_fd, channel_str))
		return (create_code_message(ERR_CHANOPRIVSNEEDED, channel_str));

	std::set<char> box;
	for (std::string::const_iterator i = mode.begin(); i != mode.end(); i++)
		box.insert(*i);
	int mod = 0;
	if (box.find('o') != box.end())
		mod += MOD_OPERATOR;
	if (box.find('k') != box.end())
		mod += MOD_KEYWORD;
	if (box.find('i') != box.end())
		mod += MOD_INVITE;
	if (box.find('t') != box.end())
		mod += MOD_TOPIC;
	if (box.find('l') != box.end())
		mod += MOD_LIMITED;
	box.erase('o');
	box.erase('k');
	box.erase('i');
	box.erase('t');
	box.erase('l');
	if (box.size() > 0)
		return (create_code_message(ERR_UNKNOWNMODE, mode));

	if (valid)
	{
		if (mod & MOD_INVITE)
			channels_[channel_str].is_invite = true;
		if (mod & MOD_TOPIC)
			channels_[channel_str].is_topic = true;
		if (mod & MOD_KEYWORD)
		{
			if (key == "")
				return (create_code_message(ERR_NEEDMOREPARAMS, "MODE"));
			channels_[channel_str].is_key = true;
			channels_[channel_str].password = key;
		}
		if (mod & MOD_LIMITED)
		{
			if (key == "")
				return (create_code_message(ERR_NEEDMOREPARAMS, "MODE"));
			channels_[channel_str].is_limit = true;
			std::stringstream str(key);
			int	num;
			if (!(str >> num) || !str.eof())
				return (ChannelResult(FATAL, ""));
			channels_[channel_str].limit_member = num;
		}
		if (mod & MOD_OPERATOR)
			ChangeOperator(player_fd, key, channel_str, 1);
	}
	else
	{
		if (mod & MOD_INVITE)
			channels_[channel_str].is_invite = false;
		if (mod & MOD_TOPIC)
			channels_[channel_str].is_topic = false;
		if (mod & MOD_KEYWORD)
			channels_[channel_str].is_key = false;
		if (mod & MOD_LIMITED)
			channels_[channel_str].is_limit = false;
		if (mod & MOD_OPERATOR)
			ChangeOperator(player_fd, key, channel_str, 0);
	}

	std::string mode_m;
	if (valid)
		mode_m = std::string("+") + mode;
	else
		mode_m = std::string("-") + mode;
	return (create_code_message(RPL_CHANNELMODEIS, channel_str, mode_m, key));
}

ChannelResult Channel::ChangeOperator(int player_fd, std::string &focas_user_str, const std::string &channel_str, bool valid)
{
	IntrusivePtr<Everyone> tmp = Everyone::GetInstance();
	if (!tmp->IsRegister(player_fd))
		return (create_code_message(ERR_NOTREGISTERED));
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (!tmp->IsAdmin(player_fd) && !IsJoined(player_fd, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if (!IsOperator(player_fd, channel_str))
		return (create_code_message(ERR_CHANOPRIVSNEEDED, channel_str));
	if (!IsJoined(tmp->GetUserIdNick(focas_user_str), channel_str))
		return (create_code_message(ERR_USERSDONTMATCH));

	if (valid)
		channels_[channel_str].is_master.insert(tmp->GetUserIdNick(focas_user_str));
	else
		channels_[channel_str].is_master.erase(tmp->GetUserIdNick(focas_user_str));
	return (ChannelResult(1, ""));
}

ChannelResult Channel::SendMessageToChannel(int player_fd, const std::string& channel_str, const std::string& message, const Sender& sender)
{
	IntrusivePtr<Everyone> eve = Everyone::GetInstance();
	if (!eve->IsRegister(player_fd))
		return (create_code_message(ERR_NOTREGISTERED));
	if (channel_str[0] == '#' || channel_str[0] == '&')
	{
		if (!ExistChannel(channel_str))
			return (create_code_message(ERR_CANNOTSENDTOCHAN, channel_str));
		if (!eve->IsAdmin(player_fd) && !IsJoined(player_fd, channel_str))
			return (create_code_message(ERR_NOTONCHANNEL, channel_str));
		for (std::set<int>::iterator it = channels_[channel_str].joined_player.begin(); it != channels_[channel_str].joined_player.end(); it++)
			sender.SendMessage(create_code_message(RPL_AWAY, eve->GetSomeone(player_fd).nick_name.back(), message), *it);
		return (create_code_message(RPL_AWAY, ":" + eve->GetSomeone(player_fd).nick_name.back() + " " + channel_str, message));
	}

	if (!eve->ExistUserNick(channel_str))
		return (create_code_message(ERR_NOSUCHNICK, channel_str));
	sender.SendMessage(create_code_message(RPL_AWAY, eve->GetSomeone(player_fd).nick_name.back(), message), eve->GetUserIdNick(channel_str));
	return (create_code_message(RPL_AWAY, ":" + eve->GetSomeone(player_fd).nick_name.back() + " " + channel_str, message));
}

bool	Channel::ExistChannel(const std::string& channel_str) const
{
	if (channels_.find(channel_str) == channels_.end())
		return (false);
	return (true);
}

bool	Channel::IsOperator(int player_fd, const std::string& channel_str) const
{
	if (Everyone::GetInstance()->IsAdmin(player_fd))
		return (true);
	if (channels_.find(channel_str)->second.is_master.find(player_fd) == channels_.find(channel_str)->second.is_master.end())
		return (false);
	return (true);
}

bool	Channel::IsJoined(int player_fd, const std::string& channel_str) const
{
	if (channels_.find(channel_str) == channels_.end())
		return (false);
	if (channels_.find(channel_str)->second.joined_player.find(player_fd) == channels_.find(channel_str)->second.joined_player.end())
		return (false);
	return (true);
}

ChannelResult	Channel::GetTopic(const std::string& channel_str)
{
	if (channels_[channel_str].topic == "")
		return (create_code_message(RPL_NOTOPIC, channel_str));
	return (create_code_message(RPL_TOPIC, channel_str, channels_[channel_str].topic));
}

void Channel::OutputLog()
{
	std::cout << "--------Channel--------" << std::endl;
	for (std::map<std::string, ChannelInfo>::iterator it = channels_.begin(); it != channels_.end(); it++)
	{
		std::cout << "channel_name: " << it->second.channel_name << std::endl;
		std::cout << "password: " << it->second.password << std::endl;
		std::cout << "topic: " << it->second.topic << std::endl;
		std::cout << "is_invite: " << it->second.is_invite << std::endl;
		std::cout << "is_topic: " << it->second.is_topic << std::endl;
		std::cout << "is_key: " << it->second.is_key << std::endl;
		std::cout << "is_limit: " << it->second.is_limit << std::endl;
		std::cout << "limit_member: " << it->second.limit_member << std::endl;
		std::cout << "joined player:";
		for (std::set<int>::iterator ite = it->second.joined_player.begin(); ite != it->second.joined_player.end(); ite++)
			std::cout << " " << *ite;
		std::cout << std::endl;
		std::cout << "is master:";
		for (std::set<int>::iterator ite = it->second.is_master.begin(); ite != it->second.is_master.end(); ite++)
			std::cout << " " << *ite;
		std::cout << std::endl;
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void Channel::Clear(int n)
{
	if (n)
		channels_.clear();
	else
	{
		IntrusivePtr<Everyone> tmp = Everyone::GetInstance();
		tmp->Clear(1);
		channels_.clear();
	}
}
