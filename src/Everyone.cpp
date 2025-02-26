#include "../includes/Everyone.h"
#include "Everyone.h"

IntrusivePtr<Everyone> Everyone::instance_ = IntrusivePtr<Everyone>(static_cast<Everyone*>(NULL));

Everyone::Everyone() : RefCounted()
{

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

ChannelResult	Everyone::CreateUser(int player_fd)
{
	if (everyone_id_.find(player_fd) != everyone_id_.end())
		return (create_code_message(ERR_ALREADYREGISTRED, everyone_id_[player_fd]->user_name));
	Someone *tmp = new Someone;
	tmp->player_fd = player_fd;
	tmp->level[NICK] = 0;
	tmp->level[USER] = 0;
	tmp->level[REGISTER] = 0;
	everyone_id_[player_fd] = tmp;
	return (ChannelResult(1, ""));
}

ChannelResult	Everyone::DeleteUser(int player_fd)
{
	Someone					*tmp = everyone_id_[player_fd];
	IntrusivePtr<Channel>	channel = Channel::GetInstance();

	for (std::set<std::string>::iterator i = tmp->join_channel.begin(); i != tmp->join_channel.end(); i++)
		channel->LeaveChannel(player_fd, *i);
	nick_list_.erase(everyone_id_[player_fd]->nick_name.back());
	Someone *del = everyone_id_[player_fd];
	delete (del);
	everyone_id_.erase(player_fd);
	return (ChannelResult(1, ""));
}

const Someone	&Everyone::GetSomeone(int player_fd) const
{
	std::map<int, Someone*>::const_iterator it = everyone_id_.find(player_fd);
	return (*it->second);
}

ChannelResult Everyone::AddJoinChannel(int player_fd, const std::string& focas)
{
	everyone_id_[player_fd]->join_channel.insert(focas);
	return (ChannelResult(1, ""));
}

ChannelResult Everyone::DeleteJoinChannel(int player_fd, const std::string& focas)
{
	everyone_id_[player_fd]->join_channel.erase(focas);
	return (ChannelResult(1, ""));
}

ChannelResult	Everyone::SetUser(int player_fd, const std::string &username, const std::string &hostname, const std::string &servername, const std::string &realname)
{
	if (IsRegister(player_fd))
		return (create_code_message(ERR_ALREADYREGISTRED));
	everyone_id_[player_fd]->user_name = username;
	everyone_id_[player_fd]->host_name = hostname;
	everyone_id_[player_fd]->server_name = servername;
	everyone_id_[player_fd]->real_name = realname;
	everyone_id_[player_fd]->level[USER] = 1;
	IsRegister(player_fd);
	return (ChannelResult(1, ""));
}

ChannelResult	Everyone::SetNickname(int player_fd, const std::string &nickname)
{
	if (nickname == "")
		return (ChannelResult(create_code_message(ERR_NONICKNAMEGIVEN)));
	if (!is_nick(nickname))
		return (ChannelResult(create_code_message(ERR_ERRONEUSNICKNAME, nickname)));
	if (nick_list_.find(nickname) != nick_list_.end())
		return (create_code_message(ERR_NICKNAMEINUSE, nickname));

	nick_list_.erase(everyone_id_[player_fd]->nick_name.back());
	nick_list_.insert(nickname);
	everyone_id_[player_fd]->nick_name.push_back(nickname);
	everyone_id_[player_fd]->level[NICK] = 1;
	IsRegister(player_fd);
	return (ChannelResult(1, ""));
}

bool	Everyone::ExistUserUser(const std::string &user_str) const
{
	std::map<std::string, Someone*>::const_iterator it = everyone_username_.find(user_str);
	if (it == everyone_username_.end())
		return (false);
	return (true);
}

bool Everyone::ExistUserNick(const std::string &nick_str) const
{
	std::map<std::string, Someone*>::const_iterator it = everyone_nickname_.find(nick_str);
	if (it == everyone_nickname_.end())
		return (false);
	return (true);
}

int Everyone::GetUserIdNick(const std::string &nick_str) const
{
	std::map<std::string, Someone*>::const_iterator it = everyone_nickname_.find(nick_str);
	return (it->second->player_fd);
}

int Everyone::GetUserIdUser(const std::string &user_str) const
{
	std::map<std::string, Someone*>::const_iterator it = everyone_username_.find(user_str);
	return (it->second->player_fd);
}

bool Everyone::IsRegister(int player_fd)
{
	if (everyone_id_.find(player_fd) == everyone_id_.end())
		return (false);
	if (everyone_id_[player_fd]->level[REGISTER])
		return (true);
	if (everyone_id_[player_fd]->level[NICK] && everyone_id_[player_fd]->level[USER])
		everyone_id_[player_fd]->level[REGISTER] = 1;
	return (false);
}
