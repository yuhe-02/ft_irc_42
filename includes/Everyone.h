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

class Channel;
struct Someone
{
	int							player_id_; // if (player_id_ == -1) this struct is error
	std::string					user_name_;
	std::string					host_name_;
	std::string					real_name_;
	std::vector<std::string>	nick_name_;
	std::set<int>				block_user_;
	std::set<int>				join_channel_;
};

class Everyone : public RefCounted
{
private:
	static IntrusivePtr<Everyone>	instance_;
	std::map<std::string, int>		everyone_string_id;
	std::map<int, Someone>			everyone_;
	long							latest_user_id_;

	Everyone();
	Everyone(const Everyone &);
	Everyone &operator=(const Everyone &);

public:
	~Everyone();

	static IntrusivePtr<Everyone>	GetInstance();
	int								CreateUser(const std::string &name);
	int								DeleteUser(int player_id);
	int								GetSomeone(int player_id, Someone &dest) const;
	int								GetSomeoneID(const std::string &plyer_name) const;
	int								AddBlockUser(int player_id, int focas);
	int								DeleteBlockUser(int player_id, int focas);
	int								AddJoinChannel(int player_id, int focas);
	int								DeleteJoinChannel(int player_id, int focas);
	int								SetHostname(int player_id, const std::string &hostname);
	int								SetRealname(int player_id, const std::string &realname);
	int								SetNickname(int player_id, const std::string &nickname);

	bool							ExistUser(int player_id) const;
};
