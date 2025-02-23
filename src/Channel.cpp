#include "../includes/Channel.h"
#include "Channel.h"

Channel::Channel() : RefCounted(), latest_channel_id_(0)
{
	channels_itos_.clear();
	channels_.clear();
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

pair<int, std::string>	Channel::CreateChannel(const std::string& name, const std::string &player_str, int mode)
{
	if (latest_channel_id_ >= INT_MAX)
		return (pair<int, std::string>(FATAL, 0));
	if (channels_.find(name) == channels_.end())
		return (pair<int, std::string>(FATAL, 0));

	ChannelInfo tmp;
	int chan_id = static_cast<int>(latest_channel_id_);

	tmp.joined_plyer.clear();
	tmp.is_master.clear();

	tmp.channel_id = chan_id;
	tmp.channel_name = name;
	tmp.is_master.insert(player_str);
	tmp.mode = mode;
	tmp.limit_member = LIMIT_MEMBER;
	JoinedChannel(player_str, name);
	channels_[name] = tmp;
	channels_itos_[chan_id] = name;
	latest_channel_id_++;
	return (std::pair<int, std::string>(1, ""));
}

pair<int, std::string>	Channel::DeleteChannel(const std::string& channel_str)
{
	if (channels_.find(channel_str) == channels_.end())
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));

	const ChannelInfo	tmp = channels_[channel_str];

	for (std::set<std::string>::iterator i = tmp.joined_plyer.begin(); i != tmp.joined_plyer.end(); i++)
		LeaveChannel(*i, channel_str);
	channels_itos_.erase(tmp.channel_id);
	channels_.erase(channel_str);
	return (pair<int, std::string>(1, ""));
}

pair<int, std::string>	Channel::GetChannelInfo(const std::string& channel_str, ChannelInfo &dest) const
{
	std::map<std::string, ChannelInfo>::const_iterator it = channels_.find(channel_str);
	if (it == channels_.end())
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));

	dest = it->second;
	return (pair<int, std::string>(1, ""));
}

pair<int, std::string>	Channel::InviteToChannel(const std::string &player_str, const std::string &focas_user_str, const std::string& channel_str)
{
	IntrusivePtr<Everyone> tmp = Everyone::GetInstance();
	if (tmp->ExistUser(focas_user_str))
	{
		tmp->release();
		return (create_code_message(ERR_NOSUCHNICK, focas_user_str));
	}
	tmp->release();
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

pair<int, std::string>	Channel::JoinedChannel(const std::string &player_str, const std::string& channel_str, int flag)
{
	if (flag < 0)
		return (std::pair<int, std::string>(FATAL, ""));
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (IsJoined(player_str, channel_str))
		return (create_code_message(ERR_USERONCHANNEL, player_str, channel_str));
	if (flag == 0 && (channels_[channel_str].mode & MOD_INVITE))
		return (create_code_message(ERR_INVITEONLYCHAN, channel_str));
	channels_[channel_str].joined_plyer.insert(player_str);
	IntrusivePtr<Everyone> tmp = Everyone::GetInstance();
	tmp->AddJoinChannel(player_str, channel_str);
	tmp->release();
	return (pair<int, std::string>(1, ""));
}

pair<int, std::string>	Channel::LeaveChannel(const std::string &player_str, const std::string& channel_str)
{
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (!IsJoined(player_str, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if (channels_[channel_str].is_master.size() == 1)
		return (pair<int, std::string>(FATAL, ""));
	channels_[channel_str].joined_plyer.erase(channel_str);
	channels_[channel_str].is_master.erase(channel_str);
	IntrusivePtr<Everyone> tmp = Everyone::GetInstance();
	tmp->DeleteJoinChannel(player_str, channel_str);
	tmp->release();
	return (pair<int, std::string>(1, ""));
}

pair<int, std::string>	Channel::KickChannel(const std::string &player_str, const std::string &focas_user_str, const std::string& channel_str)
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
	return (pair<int, std::string>(1, ""));
}

pair<int, std::string>	Channel::ChangeTopic(const std::string &player_str, const std::string& channel_str, const std::string &topic)
{
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (!IsJoined(player_str, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if ((channels_[channel_str].mode & MOD_TOPIC) && !IsOperator(player_str, channel_str))
		return (create_code_message(ERR_CHANOPRIVSNEEDED, channel_str));
	channels_[channel_str].topic = topic;
	return (pair<int, std::string>(1, ""));
}

pair<int, std::string>	Channel::AddMaster(const std::string &player_str, const std::string &focas_user_str, const std::string& channel_str)
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
	return (pair<int, std::string>(1, ""));
}

pair<int, std::string>	Channel::DeleteMaster(const std::string &player_str, const std::string &focas_user_str, const std::string& channel_str)
{
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (!IsJoined(player_str, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if (!IsJoined(focas_user_str, channel_str))
		return (create_code_message(ERR_USERNOTINCHANNEL, focas_user_str, channel_str));
	if (!IsOperator(player_str, channel_str) || !IsOperator(player_str, channel_str))
		return (std::pair<int, std::string>(FATAL, ""));
	channels_[channel_str].is_master.insert(focas_user_str);
	return (pair<int, std::string>(1, ""));
}

pair<int, std::string>	Channel::ChangeMode(const std::string &player_str, int mode, bool valid, const std::string& channel_str)
{
	if (mode >= 32)
		return (std::pair<int, std::string>(FATAL, ""));
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (!IsJoined(player_str, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	if (!IsOperator(player_str, channel_str))
		return (create_code_message(ERR_CHANOPRIVSNEEDED, channel_str));

	if (valid)
		channels_[channel_str].mode = channels_[channel_str].mode | mode;
	else
		channels_[channel_str].mode = channels_[channel_str].mode & ~mode;
	return (pair<int, std::string>(1, ""));
}

pair<int, std::string> Channel::SendMessageToChannel(const std::string &player_str, const std::string& channel_str)
{
	if (!ExistChannel(channel_str))
		return (create_code_message(ERR_NOSUCHCHANNEL, channel_str));
	if (!IsJoined(player_str, channel_str))
		return (create_code_message(ERR_NOTONCHANNEL, channel_str));
	return (pair<int, std::string>(1, ""));
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
	if (channels_.find(channel_str)->second.joined_plyer.find(player_str) == channels_.find(channel_str)->second.joined_plyer.end())
		return (false);
	return (true);
}
