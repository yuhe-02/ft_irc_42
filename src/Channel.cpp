#include "../includes/Channel.h"
#include "Channel.h"

Channel::Channel() : RefCounted(), latest_channel_id_(0)
{
	channel_string_id.clear();
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

int	Channel::CreateChannel(std::string name, int player_id, int mode)
{
	if (latest_channel_id_ >= INT_MAX)
		return (OUT_OF_RANGE);

	ChannelInfo tmp;
	int chan_id = static_cast<int>(latest_channel_id_);

	tmp.joined_plyer.clear();
	tmp.is_master.clear();

	tmp.channel_id = chan_id;
	tmp.channel_name = name;
	tmp.is_master.insert(player_id);
	tmp.mode = mode;
	tmp.limit_member = LIMIT_MEMBER;
	JoinedChannel(player_id, chan_id);
	channel_string_id[name] = chan_id;
	channels_[chan_id] = tmp;
	latest_channel_id_++;
	return (TRUE);
}

int	Channel::DeleteChannel(int channel_id)
{
	std::map<int, ChannelInfo>::const_iterator it = channels_.find(channel_id);
	if (it == channels_.end())
		return (NO_CHANNEL);

	const ChannelInfo	tmp = channels_[channel_id];

	// std::for_each(tmp.joined_plyer.begin(), tmp.joined_plyer.end(), [channel_id, this](int value){
	// 	LeaveChannel(channel_id, value);
	// });
	for (std::set<int>::iterator i = tmp.joined_plyer.begin(); i != tmp.joined_plyer.end(); i++)
		LeaveChannel(channel_id, *i);
	channel_string_id.erase(tmp.channel_name);
	channels_.erase(channel_id);
	return (TRUE);
}

int	Channel::GetChannelInfo(int channel_id, ChannelInfo &dest) const
{
	std::map<int, ChannelInfo>::const_iterator it = channels_.find(channel_id);
	if (it == channels_.end())
		return (NO_CHANNEL);

	dest = it->second;
	return (TRUE);
}

int	Channel::GetChannelID(const std::string &channel) const
{
	std::map<std::string, int>::const_iterator it = channel_string_id.find(channel);
	if (it == channel_string_id.end())
		return (NO_CHANNEL);
	return (it->second);
}

int	Channel::InviteToChannel(int from_id, int focas_id, int channel_id)
{
	if (!IsOperator(from_id, channel_id))
		return (NO_AUTHORITY);
	int err = JoinedChannel(focas_id, channel_id, 1);
	if (err < 0)
		return (err);
	return (TRUE);
}

int	Channel::JoinedChannel(int player_id, int channel_id, int flag)
{
	if (flag < 0)
		return (UNKNOWN);
	if (!ExistChannel(channel_id))
		return (NO_CHANNEL);
	if (IsJoined(player_id, channel_id))
		return (ALREADY_JOINED);
	if (flag && (channels_[channel_id].mode & MOD_INVITE))
		return (NO_AUTHORITY);
	channels_[channel_id].joined_plyer.insert(player_id);
	IntrusivePtr<Everyone> tmp = Everyone::GetInstance();
	tmp->AddJoinChannel(player_id, channel_id);
	tmp->release();
	return (TRUE);
}

int	Channel::LeaveChannel(int player_id, int channel_id)
{
	if (!ExistChannel(channel_id))
		return (NO_CHANNEL);
	if (!IsJoined(player_id, channel_id))
		return (ALREADY_LEAVE);
	if (channels_[channel_id].is_master.size() == 1)
		return (LAST_OPERATOR);
	channels_[channel_id].joined_plyer.erase(player_id);
	channels_[channel_id].is_master.erase(player_id);
	IntrusivePtr<Everyone> tmp = Everyone::GetInstance();
	tmp->DeleteJoinChannel(player_id, channel_id);
	tmp->release();
	return (TRUE);
}

int	Channel::KickChannel(int player_id, int focas_id, int channel_id)
{
	if (!ExistChannel(channel_id))
		return (NO_CHANNEL);
	if (!IsOperator(player_id, channel_id))
		return (NO_AUTHORITY);
	LeaveChannel(focas_id, channel_id);
	return (TRUE);
}

int	Channel::ChangeTopic(int player_id, int channel_id, const std::string &topic)
{
	if (!ExistChannel(channel_id))
		return (NO_CHANNEL);
	if (!IsJoined(player_id, channel_id))
		return (NO_JOINED);
	if ((channels_[channel_id].mode & MOD_TOPIC) && !IsOperator(player_id, channel_id))
		return (NO_AUTHORITY);
	channels_[channel_id].topic = topic;
	return (TRUE);
}

int	Channel::AddMaster(int player_id, int focas, int channel_id)
{
	if (!ExistChannel(channel_id))
		return (NO_CHANNEL);
	if (!IsOperator(player_id, channel_id))
		return (NO_AUTHORITY);
	if (!IsJoined(focas, channel_id))
		return (NO_JOINED);
	channels_[channel_id].is_master.insert(focas);
	return (TRUE);
}

int	Channel::DeleteMaster(int player_id, int focas, int channel_id)
{
	if (!ExistChannel(channel_id))
		return (NO_CHANNEL);
	if (!IsOperator(player_id, channel_id) || !IsOperator(focas, channel_id))
		return (NO_AUTHORITY);
	channels_[channel_id].is_master.insert(focas);
	return (TRUE);
}

int	Channel::ChangeMode(int player_id, int mode, bool valid, int channel_id)
{
	if (!ExistChannel(channel_id))
		return (NO_CHANNEL);
	if (!IsOperator(player_id, channel_id))
		return (NO_AUTHORITY);

	if (valid)
		channels_[channel_id].mode = channels_[channel_id].mode | mode;
	else
		channels_[channel_id].mode = channels_[channel_id].mode & ~mode;
	return (TRUE);
}


bool	Channel::ExistChannel(int channel_id) const
{
	std::map<int, ChannelInfo>::const_iterator it = channels_.find(channel_id);
	if (it == channels_.end())
		return (false);
	return (true);
}

bool	Channel::IsOperator(int player_id, int channel_id) const
{
	std::set<int>::iterator it = channels_.find(channel_id)->second.is_master.find(player_id);
	if (it == channels_.find(channel_id)->second.is_master.end())
		return (false);
	return (true);
}

bool	Channel::IsJoined(int player_id, int channel_id) const
{
	std::set<int>::iterator it = channels_.find(channel_id)->second.joined_plyer.find(player_id);
	if (it == channels_.find(channel_id)->second.joined_plyer.end())
		return (false);
	return (true);
}
