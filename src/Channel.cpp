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

ChannelResult	Channel::CreateChannel(int player_fd, const std::string& name, int mode)
{
	if (channels_.size() >= INT_MAX)
		return (ChannelResult(FATAL, ""));
	if (channels_.find(name) != channels_.end())
		return (ChannelResult(FATAL, ""));

	ChannelInfo tmp;

	tmp.channel_name = name;
	tmp.is_master.insert(player_fd);
	tmp.mode = mode;
	tmp.limit_member = LIMIT_MEMBER;
	channels_[name] = tmp;
	JoinedChannel(player_fd, name);
	return (ChannelResult(1, ""));
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
	IntrusivePtr<Everyone> tmp = Everyone::GetInstance();
	if (!tmp->ExistUserNick(focas_user_str))
		return (create_code_message(ERR_NOSUCHNICK, focas_user_str));
	if (!IsJoined(player_fd, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if (IsJoined(tmp->GetUserIdNick(focas_user_str), channel_str))
		return (create_code_message(ERR_USERONCHANNEL, focas_user_str, channel_str));
	if (!IsOperator(player_fd, channel_str))
		return (create_code_message(ERR_CHANOPRIVSNEEDED, channel_str));
	if (channels_[channel_str].joined_player.size() == channels_[channel_str].limit_member)
		return (create_code_message(ERR_CHANNELISFULL, channel_str));
	JoinedChannel(tmp->GetUserIdNick(focas_user_str), channel_str, 1);
	return (create_code_message(RPL_AWAY, focas_user_str));
}

ChannelResult	Channel::JoinedChannel(int player_fd, const std::string& channel_str, int flag)
{
	IntrusivePtr<Everyone> tmp = Everyone::GetInstance();
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (IsJoined(player_fd, channel_str))
		return (create_code_message(ERR_USERONCHANNEL, tmp->GetSomeone(player_fd).nick_name.back(), channel_str));
	if (flag == 0 && (channels_[channel_str].mode & MOD_INVITE))
		return (create_code_message(ERR_INVITEONLYCHAN, channel_str));
	if (channels_[channel_str].joined_player.size() == channels_[channel_str].limit_member)
		return (create_code_message(ERR_CHANNELISFULL, channel_str));
	if (tmp->GetSomeone(player_fd).join_channel.size() > 10)
		return (create_code_message(ERR_TOOMANYCHANNELS, channel_str));

	channels_[channel_str].joined_player.insert(player_fd);
	tmp->AddJoinChannel(player_fd, channel_str);
	return (create_code_message(RPL_TOPIC, channels_[channel_str].topic));
}

ChannelResult	Channel::LeaveChannel(int player_fd, const std::string& channel_str)
{
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

ChannelResult	Channel::KickChannel(int player_fd, const std::string &focas_user_str, const std::string& channel_str)
{
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (!IsJoined(player_fd, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if (!IsOperator(player_fd, channel_str))
		return (create_code_message(ERR_CHANOPRIVSNEEDED, channel_str));
	IntrusivePtr<Everyone> tmp = Everyone::GetInstance();
	LeaveChannel(tmp->GetUserIdNick(focas_user_str), channel_str);
	return (ChannelResult(1, ""));
}

ChannelResult	Channel::ChangeTopic(int player_fd, const std::string& channel_str, const std::string &topic)
{
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if (!IsJoined(player_fd, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if ((channels_[channel_str].mode & MOD_TOPIC) && !IsOperator(player_fd, channel_str))
		return (create_code_message(ERR_CHANOPRIVSNEEDED, channel_str));
	channels_[channel_str].topic = topic;
	return (ChannelResult(1, ""));
}

/// @brief
/// @param player_fd
/// @param focas_user_str
/// @param mode
/// @param valid　まだできていないよ、、、、、、、、、、、、、、、、、
/// @param channel_str
/// @return
ChannelResult Channel::ChangeUserMode(int player_fd, std::string &focas_user_str, int mode, bool valid, const std::string &channel_str)
{
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (!IsJoined(player_fd, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if (!IsOperator(player_fd, channel_str))
		return (create_code_message(ERR_CHANOPRIVSNEEDED, channel_str));
	IntrusivePtr<Everyone> tmp = Everyone::GetInstance();
	if (!tmp->ExistUserNick(focas_user_str))
		return (create_code_message(ERR_NOSUCHNICK, focas_user_str));
	return (ChannelResult(1, ""));
}

ChannelResult	Channel::ChangeChannelMode(int player_fd, int mode, bool valid, const std::string& channel_str)
{
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (!IsJoined(player_fd, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if (!IsOperator(player_fd, channel_str))
		return (create_code_message(ERR_CHANOPRIVSNEEDED, channel_str));

	if (valid)
		channels_[channel_str].mode |= (1 << mode);
	else
		channels_[channel_str].mode &= ~(1 << mode);
	return (ChannelResult(1, ""));
}

ChannelResult Channel::ChangeOperator(int player_fd, std::string &focas_user_str, const std::string &channel_str)
{
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (!IsJoined(player_fd, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if (!IsOperator(player_fd, channel_str))
		return (create_code_message(ERR_CHANOPRIVSNEEDED, channel_str));
	if (!IsJoined(player_fd, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	IntrusivePtr<Everyone> tmp = Everyone::GetInstance();
	if (IsJoined(player_fd, channel_str))
		return (create_code_message(ERR_USERSDONTMATCH, tmp->GetSomeone(player_fd).nick_name.back(), channel_str));

	if (channels_[channel_str].is_master.find(tmp->GetUserIdNick(focas_user_str)) == channels_[channel_str].is_master.end())
		channels_[channel_str].is_master.insert(tmp->GetUserIdNick(focas_user_str));
	else
		channels_[channel_str].is_master.erase(tmp->GetUserIdNick(focas_user_str));
}

ChannelResult Channel::SendMessageToChannel(int player_fd, const std::string& channel_str)
{
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (!IsJoined(player_fd, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	return (ChannelResult(1, ""));
}

bool	Channel::ExistChannel(const std::string& channel_str) const
{
	if (channels_.find(channel_str) == channels_.end())
		return (false);
	return (true);
}

/// @brief
/// @param player_fd
/// @param channel_strまだできてないよ、、、、、、、、、、、、、
/// @return
bool	Channel::IsOperator(int player_fd, const std::string& channel_str) const
{
	if (channels_.find(channel_str)->second.is_master.find(player_fd) == channels_.find(channel_str)->second.is_master.end())
		return (false);
	return (true);
}

bool	Channel::IsJoined(int player_fd, const std::string& channel_str) const
{
	if (channels_.find(channel_str)->second.joined_player.find(player_fd) == channels_.find(channel_str)->second.joined_player.end())
		return (false);
	return (true);
}
