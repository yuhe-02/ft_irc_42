#pragma once

#include "Everyone.h"
#include "RefCounted.h"
#include "IntrusivePtr.h"

#include <vector>
#include <set>
#include <map>
#include <bitset>

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

class Channel : public RefCounted
{
private:
	static IntrusivePtr<Channel>	instance_;
	std::map<std::string, int>		channel_string_id;
	std::map<int, ChannelInfo>		channels_;
	int								latest_channel_id_;

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

	bool				CreateChannel(std::string name);
	bool				DeleteChannel(int channel_id);
	const ChannelInfo	GetChannelInfo(int channel_id) const;
	int					GetChannelID(const std::string &channel) const;
	bool				InviteToChannel(int from_id, int focas_id, int channel_id);
	bool				JoinedChannel(int plyer_id, int channel_id);// Someoneの内容も変更すること(structにchannelの項目がある)
	bool				LeaveChannel(int plyer_id, int channel_id);// Someoneの内容も変更すること(structにchannelの項目がある)
	bool				KickChannel(int plyer_id, int channel_id);
	bool				ChangeTopic(int player_id, int channel_id, const std::string &topic);
	bool				AddMaster(int plyer_id, int channel_id);
	bool				DeleteMaster(int plyer_id, int channel_id);
	bool				AddVoice(int plyer_id, int channel_id);
	bool				DeleteVoice(int plyer_id, int channel_id);
	bool				ChangeMode(ChannelMode mode, bool valid, int channel_id);
};

