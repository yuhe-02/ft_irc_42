#include "../includes/Everyone.h"
#include "Everyone.h"


Everyone::Everyone() : RefCounted(), latest_user_id_(0)
{
	everyone_string_id.clear();
	everyone_.clear();
}

Everyone::Everyone(const Everyone &) : RefCounted()
{

}

Everyone &Everyone::operator=(const Everyone &)
{
	return (*this);
}

Everyone::~Everyone()
{

}

IntrusivePtr<Everyone>	Everyone::GetInstance()
{
	if (!instance_) {
		instance_ = IntrusivePtr<Everyone>(new Everyone());
	}
	return instance_;
}

int	Everyone::CreateUser(const std::string &name)
{
	if (latest_user_id_ >= INT_MAX)
		return (OUT_OF_RANGE);

	Someone tmp;
	int user_id = static_cast<int>(latest_user_id_);

	tmp.block_user_.clear();
	tmp.join_channel_.clear();
	tmp.player_id_ = user_id;
	tmp.user_name_ = name;
	everyone_string_id[name] = user_id;
	everyone_[user_id] = tmp;
	latest_user_id_++;
	return (TRUE);
}

int	Everyone::DeleteUser(int player_id)
{
	if (ExistUser(player_id) <= 0)
		return (OUT_OF_RANGE);

	const Someone			tmp = everyone_[player_id];
	IntrusivePtr<Channel>	channel = Channel::GetInstance();

	for (std::set<int>::iterator i = tmp.join_channel_.begin(); i != tmp.join_channel_.end(); i++)
		channel->LeaveChannel(player_id, *i);
	channel->release();
	everyone_string_id.erase(tmp.user_name_);
	everyone_.erase(player_id);
	return (TRUE);
}

int	Everyone::GetSomeone(int player_id, Someone &dest) const
{
	std::map<int, Someone>::const_iterator it = everyone_.find(player_id);
	if (it == everyone_.end())
		return (NO_USER);

	dest = it->second;
	return (TRUE);
}

int	Everyone::GetSomeoneID(const std::string &plyer_name) const
{
	std::map<std::string, int>::const_iterator it = everyone_string_id.find(plyer_name);
	if (it == everyone_string_id.end())
		return (NO_USER);
	return (it->second);
}

int	Everyone::AddBlockUser(int player_id, int focas)
{
	if (!ExistUser(player_id) || !ExistUser(focas))
		return (NO_USER);
	everyone_[player_id].block_user_.insert(focas);
	return (TRUE);
}

int Everyone::AddJoinChannel(int player_id, int focas)
{
	if (!ExistUser(player_id))
		return (NO_USER);
	everyone_[player_id].join_channel_.insert(focas);
	return (TRUE);
}

int Everyone::DeleteJoinChannel(int player_id, int focas)
{
	if (!ExistUser(player_id))
		return (NO_USER);
	everyone_[player_id].join_channel_.erase(focas);
	return (TRUE);
}

int	Everyone::DeleteBlockUser(int player_id, int focas)
{
	if (!ExistUser(player_id) || !ExistUser(focas))
		return (NO_USER);
	everyone_[player_id].block_user_.erase(focas);
	return (TRUE);
}

int	Everyone::SetHostname(int player_id, const std::string &hostname)
{
	if (!ExistUser(player_id))
		return (NO_USER);
	everyone_[player_id].host_name_ = hostname;
	return (TRUE);
}

int	Everyone::SetRealname(int player_id, const std::string &realname)
{
	if (!ExistUser(player_id))
		return (NO_USER);
	everyone_[player_id].real_name_ = realname;
	return (TRUE);
}

int	Everyone::SetNickname(int player_id, const std::string &nickname)
{
	if (!ExistUser(player_id))
		return (NO_USER);
	everyone_[player_id].nick_name_.push_back(nickname);
	return (TRUE);
}

bool	Everyone::ExistUser(int player_id) const
{
	std::map<int, Someone>::const_iterator it = everyone_.find(player_id);
	if (it == everyone_.end())
		return (false);
	return (true);
}
