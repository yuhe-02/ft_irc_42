#pragma once

#include "RefCounted.h"
#include "IntrusivePtr.h"
#include "ErrorCode.h"
#include "Channel.h"

#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <climits>
#include <utility>
#include <sstream>

class Channel;

typedef std::pair<int, std::string> ChannelResult;

struct Someone
{
	int							player_id; // if (player_id == -1) this struct is error
	std::string					user_name;
	std::string					host_name;
	std::string					real_name;
	std::vector<std::string>	nick_name;
	std::set<std::string>		block_user;
	std::set<std::string>		join_channel;
	std::string					password;
};

class Everyone : public RefCounted
{
private:
	static IntrusivePtr<Everyone>	instance_;
	std::map<std::string, Someone>	everyone_;
	std::map<int, std::string>		everyone_itos_;
	std::set<string>				nick_list;
	long							latest_user_id_;

	Everyone();
	Everyone(const Everyone &);
	Everyone &operator=(const Everyone &);

public:
	~Everyone();

	static IntrusivePtr<Everyone>	GetInstance();
	pair<int, std::string>			CreateUser(const std::string &username, const std::string &password, const std::string &hostname, const std::string &nickname = "", const std::string &realname = "");
	pair<int, std::string>			DeleteUser(const std::string &player_str);
	pair<int, std::string>			GetSomeone(const std::string &player_str, Someone &dest) const;
	pair<int, std::string>			AddBlockUser(const std::string &player_str, const std::string& focas);
	pair<int, std::string>			DeleteBlockUser(const std::string &player_str, const std::string& focas);
	pair<int, std::string>			AddJoinChannel(const std::string &player_str, const std::string& focas);
	pair<int, std::string>			DeleteJoinChannel(const std::string &player_str, const std::string& focas);
	pair<int, std::string>			SetUsername(const std::string &player_str, const std::string &username);
	pair<int, std::string>			SetHostname(const std::string &player_str, const std::string &hostname);
	pair<int, std::string>			SetRealname(const std::string &player_str, const std::string &realname);
	pair<int, std::string>			SetNickname(const std::string &player_str, const std::string &nickname);
	pair<int, std::string>			SetPassword(const std::string &player_str, const std::string &password);

	bool							ExistUser(const std::string &player_str) const;
};
