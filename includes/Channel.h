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
#define MOD_LIMITED 8	//0d01000
#define MOD_OPERATOR 16	//0d10000

class Everyone;

struct ChannelInfo
{
	std::string				channel_name;
	std::string				password;
	std::string				topic;
	bool					is_invite;
	bool					is_topic;
	bool					is_key;
	bool					is_limit;
	std::set<int>			joined_player;
	std::set<int>			is_master;
	int						limit_member;
};

class Channel : public RefCounted
{
private:
	static IntrusivePtr<Channel>		instance_;
	std::map<std::string, ChannelInfo>	channels_;

	Channel();
	Channel(const Channel &);
	Channel &operator=(const Channel &);

public:
	~Channel();

	static IntrusivePtr<Channel>	GetInstance();
	ChannelResult					CreateChannel(int player_fd, const std::string &channel_str);
	ChannelResult					DeleteChannel(const std::string& channel_str);
	const ChannelInfo&				GetChannelInfo(const std::string& channel_str) const;
	ChannelResult					InviteToChannel(int player_fd, const std::string &focas_user_str, const std::string& channel_str);
	ChannelResult					JoinedChannel(int player_fd, const std::string& channel_str, int flag = 0, std::string pass = "");
	ChannelResult					LeaveChannel(int player_fd, const std::string& channel_str);
	ChannelResult					KickChannel(int player_fd, const std::string &focas_user_str, const std::string& channel_str);
	ChannelResult					ChangeTopic(int player_fd, const std::string& channel_str, const std::string &topic);
	ChannelResult					ChangeChannelMode(int player_fd, std::string &focas_user_str, std::string& mode,
										bool valid, const std::string& channel_str, std::string key = "");
	ChannelResult					SendMessageToChannel(int player_fd, const std::string& channel_str);
	ChannelResult					ChangeOperator(int player_fd, std::string &focas_user_str, const std::string& channel_str, bool valid);

	bool							ExistChannel(const std::string& channel_str) const;
	bool							IsOperator(int player_fd, const std::string& channel_str) const;
	bool							IsJoined(int player_fd, const std::string& channel_str) const;
};

