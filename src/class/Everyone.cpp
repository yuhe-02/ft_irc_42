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

ChannelResult	Everyone::CreateUser(int player_fd, int flag)
{
	if (everyone_id_.find(player_fd) != everyone_id_.end())
		return (create_code_message(ERR_ALREADYREGISTRED, everyone_id_[player_fd]->user_name));
	Someone *tmp = new Someone;
	tmp->player_fd = player_fd;
	tmp->level[NICK] = 0;
	tmp->level[USER] = 0;
	tmp->level[REGISTER] = 0;
	if (flag)
		tmp->is_admin = true;
	else
		tmp->is_admin = false;
	everyone_id_[player_fd] = tmp;
	return (ChannelResult(1, "001"));
}

ChannelResult	Everyone::DeleteUser(int player_fd)
{
	if (!IsCreated(player_fd))
		return (ChannelResult(FATAL, ""));
	Someone					*tmp = everyone_id_[player_fd];
	IntrusivePtr<Channel>	channel = Channel::GetInstance();

	std::set<std::string> copy = tmp->join_channel;
	for (std::set<std::string>::iterator i = copy.begin(); i != copy.end(); i++)
		channel->LeaveChannel(player_fd, *i);
	if (everyone_id_[player_fd]->nick_name.size())
		nick_list_.erase(everyone_id_[player_fd]->nick_name.back());
	everyone_nickname_.erase(everyone_id_[player_fd]->nick_name.back());
	everyone_username_.erase(everyone_id_[player_fd]->user_name);
	Someone *del = everyone_id_[player_fd];
	delete (del);
	everyone_id_.erase(player_fd);
	return (ChannelResult(-1, "001"));
}

const Someone	&Everyone::GetSomeone(int player_fd) const
{
	std::map<int, Someone*>::const_iterator it = everyone_id_.find(player_fd);
	return (*it->second);
}

ChannelResult Everyone::AddJoinChannel(int player_fd, const std::string& focas)
{
	if (!IsCreated(player_fd))
		return (ChannelResult(FATAL, ""));
	everyone_id_[player_fd]->join_channel.insert(focas);
	return (ChannelResult(1, "001"));
}

ChannelResult Everyone::DeleteJoinChannel(int player_fd, const std::string& focas)
{
	if (!IsCreated(player_fd))
		return (ChannelResult(FATAL, ""));
	everyone_id_[player_fd]->join_channel.erase(focas);
	return (ChannelResult(1, "001"));
}

ChannelResult	Everyone::SetUser(int player_fd, const std::string &username, const std::string &hostname, const std::string &servername, const std::string &realname)
{
	if (!IsCreated(player_fd))
	{
		return (ChannelResult(FATAL, ""));
	}
	// user,nick登録済み、passなしの場合はエラー
	// nick未登録の場合はスルー（pass拘らず）
	// それ以外の場合はwelcome
	if (everyone_id_[player_fd]->level[NICK] && !everyone_id_[player_fd]->level[REGISTER])
	{
		// TODO 切断動作をする
		return ChannelResult(-1, "ERROR: Access denied: Bad password?");
	}
	everyone_id_[player_fd]->user_name = username;
	everyone_id_[player_fd]->host_name = hostname;
	everyone_id_[player_fd]->server_name = servername;
	everyone_id_[player_fd]->real_name = realname;
	everyone_username_[username] = everyone_id_[player_fd];
	if (!everyone_id_[player_fd]->level[NICK]) {
		// welcome message前のバリデーション
		everyone_id_[player_fd]->level[USER] = 1;
		return ChannelResult(RPL_NOSEND, "No need to reply");
	} else if (!everyone_id_[player_fd]->level[USER]) {
		// Welcomeメッセージ送信
		everyone_id_[player_fd]->level[USER] = 1;
		return (create_code_message(1));
	}
	// このreturnが走ることはない
	return ChannelResult(RPL_NOSEND, "No need to reply");
}

ChannelResult	Everyone::SetNickname(int player_fd, const std::string &nickname)
{
	if (!IsCreated(player_fd))
	{
		return (ChannelResult(FATAL, ""));
	} else if (nickname == "")
	{
		return (ChannelResult(create_code_message(ERR_NONICKNAMEGIVEN)));
	} else if (!is_nick(nickname))
	{
		return (ChannelResult(create_code_message(ERR_ERRONEUSNICKNAME, nickname)));
	} else if (nick_list_.find(nickname) != nick_list_.end())
	{
		return (create_code_message(ERR_NICKNAMEINUSE, nickname));
	}

	if (everyone_id_[player_fd]->level[USER] && !everyone_id_[player_fd]->level[REGISTER])
	{
		// TODO 切断動作をする
		// welcome message前のバリデーション
		return ChannelResult(-1, "ERROR: Access denied: Bad password?");
	}
	// ニックネームを変更する場合は既存の値を削除する
	if (everyone_id_[player_fd]->nick_name.size() > 0)
	{
		nick_list_.erase(everyone_id_[player_fd]->nick_name.back());
	}
	nick_list_.insert(nickname);
	everyone_id_[player_fd]->nick_name.push_back(nickname);
	everyone_nickname_[nickname] = everyone_id_[player_fd];
	if (!everyone_id_[player_fd]->level[USER]) {
		// welcome message前のバリデーション
		everyone_id_[player_fd]->level[NICK] = 1;
		return ChannelResult(RPL_NOSEND, "No need to reply");
	} else if (!everyone_id_[player_fd]->level[NICK]) {
		// Welcomeメッセージ送信
		everyone_id_[player_fd]->level[NICK] = 1;
		return (create_code_message(1));
	}
	return ChannelResult(RPL_NOSEND, "No need to reply");
}

ChannelResult	Everyone::SetRegister(int player_fd, int flag)
{
	if (!IsCreated(player_fd))
		return (ChannelResult(FATAL, ""));
	if (everyone_id_[player_fd]->level[REGISTER])
		return (create_code_message(ERR_ALREADYREGISTRED));
	everyone_id_[player_fd]->level[REGISTER] = flag;
	return (ChannelResult(1, "001"));
}

void Everyone::OutputLog()
{
	std::cout << "--------USERS--------" << std::endl;
	for (std::map<int, Someone*>::iterator it = everyone_id_.begin(); it != everyone_id_.end(); it++)
	{
		std::cout << "player_fd: " << it->second->player_fd << std::endl;
		std::cout << "level: " << it->second->level[0] << ", " << it->second->level[1] << ", " << it->second->level[2] << std::endl;
		std::cout << "user_name: " << it->second->user_name << std::endl;
		std::cout << "host_name: " << it->second->host_name << std::endl;
		std::cout << "server_name: " << it->second->server_name << std::endl;
		std::cout << "real_name: " << it->second->real_name << std::endl;
		std::cout << "nick name:";
		for (std::vector<std::string>::iterator ite = it->second->nick_name.begin(); ite != it->second->nick_name.end(); ite++)
			std::cout << " " << *ite;
		std::cout << std::endl;
		std::cout << "join channel:";
		for (std::set<std::string>::iterator ite = it->second->join_channel.begin(); ite != it->second->join_channel.end(); ite++)
			std::cout << " " << *ite;
		std::cout << std::endl;
		std::cout << "is_admin: " << it->second->is_admin << std::endl << std::endl;
	}
	std::cout << "--------nicklist--------" << std::endl;
	for (std::set<std::string>::iterator ite = nick_list_.begin(); ite != nick_list_.end(); ite++)
		std::cout << " " << *ite;
	std::cout << std::endl << std::endl;
}

void Everyone::SendLog(std::string nick, int player_fd)
{
	Sender sender;
	if (!ExistUserNick(nick))
	{
		sender.SendMessage(create_code_message(ERR_NOSUCHNICK), player_fd);
		sender.SendMessage(create_code_message(RPL_ENDOFWHOIS), player_fd);
		return ;
	}
	Someone some = *everyone_id_[GetUserIdNick(nick)];
	sender.SendMessage(create_code_message(RPL_WHOISUSER, nick, some.user_name, some.host_name, some.real_name), player_fd);
	std::string ms(nick + ":");
	for (std::set<std::string>::iterator it = some.join_channel.begin(); it != some.join_channel.end(); it++)
		ms += *it + " ";
	sender.SendMessage(ChannelResult(RPL_WHOISCHANNELS, ms), player_fd);
	sender.SendMessage(ChannelResult(RPL_WHOISSERVER, ""), player_fd);
	sender.SendMessage(create_code_message(RPL_ENDOFWHOIS, nick), player_fd);
}

void Everyone::Clear(int n)
{
	if (n)
	{
		everyone_nickname_.clear();
		everyone_username_.clear();
		nick_list_.clear();
		for (std::map<int, Someone *>::iterator i = everyone_id_.begin(); i != everyone_id_.end(); i++)
			delete (i->second);
		everyone_id_.clear();
	}
	else
	{
		IntrusivePtr<Channel> tmp = Channel::GetInstance();
		tmp->Clear(1);
		everyone_nickname_.clear();
		everyone_username_.clear();
		nick_list_.clear();
		for (std::map<int, Someone *>::iterator i = everyone_id_.begin(); i != everyone_id_.end(); i++)
			delete (i->second);
		everyone_id_.clear();
	}
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
	if (!IsCreated(player_fd))
		return (false);
	if (everyone_id_[player_fd]->level[REGISTER])
		return (true);
	return (false);
}

bool Everyone::IsRegisterNick(int player_fd)
{
	if (!IsCreated(player_fd))
		return (false);
	if (everyone_id_[player_fd]->level[NICK])
		return (true);
	return (false);
}

bool Everyone::IsRegisterUser(int player_fd)
{
	if (!IsCreated(player_fd))
		return (false);
	if (everyone_id_[player_fd]->level[USER])
		return (true);
	return (false);
}

bool Everyone::IsRegisterAll(int player_fd)
{
	if (!IsCreated(player_fd))
		return (false);
	const Someone *man = everyone_id_[player_fd];
	return (man->level[USER] && man->level[REGISTER] && man->level[NICK]);
}

bool Everyone::IsCreated(int player_fd)
{
	return (everyone_id_.find(player_fd) != everyone_id_.end());
}

bool Everyone::IsAdmin(int player_fd)
{
	if (!IsCreated(player_fd))
		return (false);
	return (everyone_id_[player_fd]->is_admin);
}
