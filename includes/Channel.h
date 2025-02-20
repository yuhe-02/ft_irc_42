#pragma once

#include "Everyone.h"
#include "RefCounted.h"
#include "IntrusivePtr.h"

#include <vector>
#include <set>
#include <map>
#include <bitset>

class Channel : public RefCounted
{
private:
	enum	ChannelMode
	{
		INVITE,
		TOPIC,
		KEYWORD,
		OPERATOR,
		LOCKED
	};
	struct ChannelInfo
	{
		int					channel_id;
		std::string			channel_name;
		std::set<int>		joined_plyer;
		std::string			topic;
		std::bitset<5>		mode;
		std::set<int>		is_master;
		std::set<int>		can_voice;
	};
	static IntrusivePtr<Channel>		instance_;
	std::map<std::string, int>	channel_string_id;
	std::map<int, ChannelInfo>	channels_;
	int							latest_channel_id_;
	Channel();
	Channel(const Channel &other);
	Channel &operator=(const Channel &other);

public:
	~Channel();
	static IntrusivePtr<Channel>	GetInstance()
	{
		if (!instance_) {
			instance_ = IntrusivePtr<Channel>(new Channel());
		}
		return instance_;
	}

	bool		CreateChannel();
	bool		DeleteChannel(int channel_id);
	ChannelInfo	GetChannelInfo(int channel_id) const;
	int			GetChannelId(const std::string &channel) const;
	bool		InviteToChannel(int from_id, int focas_id);
	bool		JoinedChannel(int plyer_id, int channel_id);
	bool		LeaveChannel(int plyer_id, int channel_id);
	bool		KickChannel(int plyer_id, int channel_id);
};

