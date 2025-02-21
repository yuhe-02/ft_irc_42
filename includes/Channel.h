#pragma once

#include "Everyone.h"
#include "RefCounted.h"
#include "IntrusivePtr.h"
#include "ErrorCode.h"

#include <vector>
#include <set>
#include <map>
#include <bitset>
#include <climits>

#define LIMIT_MEMBER 10000
#define MOD_INVITE 1	//0d00001
#define MOD_TOPIC 2		//0d00010
#define MOD_KEYWORD 4	//0d00100
#define MOD_OPERATOR 8	//0d01000
#define MOD_LIMITED 16	//0d10000

class Everyone;

struct ChannelInfo
{
	int				channel_id;
	std::string		channel_name;
	std::string		password;
	std::string		topic;
	int				mode;
	std::set<int>	joined_plyer;
	std::set<int>	is_master;
	int				limit_member;
};

class Channel : public RefCounted
{
private:
	static IntrusivePtr<Channel>	instance_;
	std::map<std::string, int>		channel_string_id;
	std::map<int, ChannelInfo>		channels_;
	long							latest_channel_id_;

	Channel();
	Channel(const Channel &);
	Channel &operator=(const Channel &);

public:
	~Channel();

	static IntrusivePtr<Channel>	GetInstance();
	int								CreateChannel(std::string name, int player_id, int mode);
	int								DeleteChannel(int channel_id);
	int								GetChannelInfo(int channel_id, ChannelInfo &dest) const;
	int								GetChannelID(const std::string &channel) const;
	int								InviteToChannel(int from_id, int focas_id, int channel_id);
	int								JoinedChannel(int plyer_id, int channel_id, int flag = 0);// Someoneの内容も変更すること(structにchannelの項目がある)
	int								LeaveChannel(int plyer_id, int channel_id);// Someoneの内容も変更すること(structにchannelの項目がある)
	int								KickChannel(int player_id, int focas, int channel_id);
	int								ChangeTopic(int player_id, int channel_id, const std::string &topic);
	int								AddMaster(int player_id, int focas, int channel_id);
	int								DeleteMaster(int player_id, int focas, int channel_id);
	int								ChangeMode(int player_id, int mode, bool valid, int channel_id);

	bool							ExistChannel(int channel_id) const;
	bool							IsOperator(int plyer_id, int channel_id) const;
	bool							IsJoined(int plyer_id, int channel_id) const;
};

