#pragma once

#include "RefCounted.h"
#include "IntrusivePtr.h"

#include <string>
#include <vector>
#include <map>
#include <set>

struct Someone
{
	int							plyer_id_;
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
	int								latest_channel_id_;

	Everyone();
	Everyone(const Everyone &other);
	Everyone &operator=(const Everyone &other);

public:
	~Everyone();

	static IntrusivePtr<Everyone>	GetInstance()
	{
		if (!instance_) {
			instance_ = IntrusivePtr<Everyone>(new Everyone());
		}
		return instance_;
	}

	bool			CreateUser(std::string name);
	bool			DeleteUser(int plyer_id);
	const Someone&	GetSomeone(int plyer_id) const;
	int				GetSomeoneID(const std::string &plyer_name) const;
	bool			AddBlockUser(int plyer_id, int focas);
	bool			DeleteBlockUser(int plyer_id, int focas);
	bool			SetHostname(std::string hostname);
	bool			SetRealname(std::string realname);
	bool			SetNickname(std::string nickname);
};
