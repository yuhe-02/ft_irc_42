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
#include <iostream>

class Channel;

enum FLAGREGISTER
{
	NICK = 0,
	USER = 1,
	REGISTER = 2,
};

struct Someone
{
	int							player_fd;
	int							level[3];
	std::string					user_name;
	std::string					host_name;
	std::string					server_name;
	std::string					real_name;
	std::vector<std::string>	nick_name;
	std::set<std::string>		join_channel;
	bool						is_admin;
};

class Everyone : public RefCounted
{
private:
	static IntrusivePtr<Everyone>	instance_;
	std::map<std::string, Someone*>	everyone_username_;
	std::map<std::string, Someone*>	everyone_nickname_;
	std::map<int, Someone*>			everyone_id_;
	std::set<std::string>			nick_list_;

	Everyone();
	Everyone(const Everyone &);
	Everyone &operator=(const Everyone &);

public:
	~Everyone();
	static IntrusivePtr<Everyone>	GetInstance();
	ChannelResult					CreateUser(int player_fd, int flag = 0);
	ChannelResult					DeleteUser(int player_fd);
	const Someone&					GetSomeone(int player_fd) const;
	ChannelResult					AddJoinChannel(int player_fd, const std::string& focas);
	ChannelResult					DeleteJoinChannel(int player_fd, const std::string& focas);
	ChannelResult					SetUser(int player_fd, const std::string &username, const std::string &hostname, const std::string &servername, const std::string &realname);
	ChannelResult					SetNickname(int player_fd, const std::string &nickname);
	void							OutputLog();
	void							Clear(int n);

	int								GetUserIdNick(const std::string &nick_str) const;
	int								GetUserIdUser(const std::string &user_str) const;
	bool							ExistUserUser(const std::string &user_str) const;
	bool							ExistUserNick(const std::string &user_str) const;
	bool							IsRegister(int player_fd);
};

