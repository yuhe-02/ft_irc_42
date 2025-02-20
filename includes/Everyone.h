#pragma once

#include "RefCounted.h"
#include "IntrusivePtr.h"

#include <string>
#include <vector>
#include <map>
#include <set>

class Everyone : public RefCounted
{

private:
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
	static IntrusivePtr<Everyone>		instance_;
	std::map<std::string, int>	everyone_string_id;
	std::map<int, Someone>		everyone_;
	int							latest_channel_id_;
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
	const Someone&			GetSomeone(int plyer_id) const;
	int						GetSomeoneID(const std::string &plyer_name) const;
	bool					AddBlockUser(int plyer_id, int focas);
	bool					DeleteBlockUser(int plyer_id, int focas);
	bool					AddMaster(int plyer_id, int channel_id);
	bool					DeleteMaster(int plyer_id, int channel_id);
	bool					AddVoice(int plyer_id, int channel_id);
	bool					DeleteVoice(int plyer_id, int channel_id);
};
