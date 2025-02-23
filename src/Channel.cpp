#include "Channel.h"

IntrusivePtr<Channel> Channel::instance_ = IntrusivePtr<Channel>(static_cast<Channel*>(NULL));

Channel::Channel() : RefCounted(), latest_channel_id_(0)
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

ChannelResult	Channel::CreateChannel(const std::string& name, const std::string &player_str, int mode)
{
	if (latest_channel_id_ >= INT_MAX)
		return (ChannelResult(FATAL, ""));
	if (channels_.find(name) != channels_.end())
		return (ChannelResult(FATAL, ""));

	ChannelInfo tmp;
	int chan_id = static_cast<int>(latest_channel_id_);

	tmp.channel_id = chan_id;
	tmp.channel_name = name;
	tmp.is_master.insert(player_str);
	tmp.mode = mode;
	tmp.limit_member = LIMIT_MEMBER;
	JoinedChannel(player_str, name);
	channels_[name] = tmp;
	channels_itos_[chan_id] = name;
	latest_channel_id_++;
	return (ChannelResult(1, ""));
}

ChannelResult	Channel::DeleteChannel(const std::string& channel_str)
{
	if (channels_.find(channel_str) == channels_.end())
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));

	const ChannelInfo	tmp = channels_[channel_str];

	for (std::set<std::string>::iterator i = tmp.joined_player.begin(); i != tmp.joined_player.end(); i++)
		LeaveChannel(*i, channel_str);
	channels_itos_.erase(tmp.channel_id);
	channels_.erase(channel_str);
	return (ChannelResult(1, ""));
}

ChannelResult	Channel::GetChannelInfo(const std::string& channel_str, ChannelInfo &dest) const
{
	std::map<std::string, ChannelInfo>::const_iterator it = channels_.find(channel_str);
	if (it == channels_.end())
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));

	dest = it->second;
	return (ChannelResult(1, ""));
}

ChannelResult	Channel::InviteToChannel(const std::string &player_str, const std::string &focas_user_str, const std::string& channel_str)
{
	IntrusivePtr<Everyone> tmp = Everyone::GetInstance();
	if (!tmp->ExistUser(focas_user_str))
		return (create_code_message(ERR_NOSUCHNICK, focas_user_str));
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (!IsJoined(player_str, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if (!IsOperator(player_str, channel_str))
		return (create_code_message(ERR_CHANOPRIVSNEEDED, channel_str));
	if (IsJoined(focas_user_str, channel_str))
		return (create_code_message(ERR_USERONCHANNEL, focas_user_str, channel_str));
	return (JoinedChannel(focas_user_str, channel_str, 1));
}

ChannelResult	Channel::JoinedChannel(const std::string &player_str, const std::string& channel_str, int flag)
{
	if (flag < 0)
		return (ChannelResult(FATAL, ""));
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (IsJoined(player_str, channel_str))
		return (create_code_message(ERR_USERONCHANNEL, player_str, channel_str));
	if (flag == 0 && (channels_[channel_str].mode & MOD_INVITE))
		return (create_code_message(ERR_INVITEONLYCHAN, channel_str));
	channels_[channel_str].joined_player.insert(player_str);
	IntrusivePtr<Everyone> tmp = Everyone::GetInstance();
	tmp->AddJoinChannel(player_str, channel_str);
	return (ChannelResult(1, ""));
}

ChannelResult	Channel::LeaveChannel(const std::string &player_str, const std::string& channel_str)
{
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (!IsJoined(player_str, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if (channels_[channel_str].is_master.size() == 1 && channels_[channel_str].is_master.find(player_str) != channels_[channel_str].is_master.end())
		return (ChannelResult(FATAL, ""));
	channels_[channel_str].joined_player.erase(player_str);
	if (channels_[channel_str].joined_player.size() == 0)
	{
		channels_itos_.erase(channels_[channel_str].channel_id);
		channels_.erase(channel_str);
	}
	channels_[channel_str].is_master.erase(player_str);
	IntrusivePtr<Everyone> tmp = Everyone::GetInstance();
	tmp->DeleteJoinChannel(player_str, channel_str);
	return (ChannelResult(1, ""));
}

ChannelResult	Channel::KickChannel(const std::string &player_str, const std::string &focas_user_str, const std::string& channel_str)
{
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (!IsJoined(player_str, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if (!IsOperator(player_str, channel_str))
		return (create_code_message(ERR_CHANOPRIVSNEEDED, channel_str));
	if (!IsJoined(focas_user_str, channel_str))
		return (create_code_message(ERR_USERNOTINCHANNEL, focas_user_str, channel_str));
	LeaveChannel(focas_user_str, channel_str);
	return (ChannelResult(1, ""));
}

ChannelResult	Channel::ChangeTopic(const std::string &player_str, const std::string& channel_str, const std::string &topic)
{
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (!IsJoined(player_str, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if ((channels_[channel_str].mode & MOD_TOPIC) && !IsOperator(player_str, channel_str))
		return (create_code_message(ERR_CHANOPRIVSNEEDED, channel_str));
	channels_[channel_str].topic = topic;
	return (ChannelResult(1, ""));
}

ChannelResult	Channel::AddMaster(const std::string &player_str, const std::string &focas_user_str, const std::string& channel_str)
{
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (!IsJoined(player_str, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if (!IsOperator(player_str, channel_str))
		return (create_code_message(ERR_CHANOPRIVSNEEDED, channel_str));
	if (!IsJoined(focas_user_str, channel_str))
		return (create_code_message(ERR_USERNOTINCHANNEL, focas_user_str, channel_str));
	channels_[channel_str].is_master.insert(focas_user_str);
	return (ChannelResult(1, ""));
}

ChannelResult	Channel::DeleteMaster(const std::string &player_str, const std::string &focas_user_str, const std::string& channel_str)
{
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (!IsJoined(player_str, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if (!IsJoined(focas_user_str, channel_str))
		return (create_code_message(ERR_USERNOTINCHANNEL, focas_user_str, channel_str));
	if (!IsOperator(player_str, channel_str))
		return (create_code_message(ERR_CHANOPRIVSNEEDED, channel_str));
	channels_[channel_str].is_master.erase(focas_user_str);
	return (ChannelResult(1, ""));
}

ChannelResult	Channel::ChangeMode(const std::string &player_str, int mode, bool valid, const std::string& channel_str)
{
	if (mode >= 32 || mode < 0)
		return (ChannelResult(FATAL, ""));
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (!IsJoined(player_str, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if (!IsOperator(player_str, channel_str))
		return (create_code_message(ERR_CHANOPRIVSNEEDED, channel_str));

	if (valid)
		channels_[channel_str].mode |= (1 << mode);
	else
		channels_[channel_str].mode &= ~(1 << mode);
	return (ChannelResult(1, ""));
}

ChannelResult Channel::SendMessageToChannel(const std::string &player_str, const std::string& channel_str)
{
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (!IsJoined(player_str, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	return (ChannelResult(1, ""));
}

bool	Channel::ExistChannel(const std::string& channel_str) const
{
	if (channels_.find(channel_str) == channels_.end())
		return (false);
	return (true);
}

bool	Channel::IsOperator(const std::string &player_str, const std::string& channel_str) const
{
	if (channels_.find(channel_str)->second.is_master.find(player_str) == channels_.find(channel_str)->second.is_master.end())
		return (false);
	return (true);
}

bool	Channel::IsJoined(const std::string &player_str, const std::string& channel_str) const
{
	if (channels_.find(channel_str)->second.joined_player.find(player_str) == channels_.find(channel_str)->second.joined_player.end())
		return (false);
	return (true);
}
