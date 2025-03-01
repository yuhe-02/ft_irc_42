#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <utility>

#include "ErrorCode.h"
#include "Channel.h"
#include "Everyone.h"
#include "Sender.hpp"

#define ADMIN 42

class MessageTranslator
{
private:
	std::string pass_;
	std::string operator_pass_;
	std::map<std::string, void (MessageTranslator::*)(std::vector<std::string>, int)> func_;
	IntrusivePtr<Channel>	channel_;
	IntrusivePtr<Everyone>	user_;
	Sender					sender_;

	std::vector<std::string>	Translate(std::string str);
	void						Unknown(std::vector<std::string> av, int player_fd);
	void						Pass(std::vector<std::string> av, int player_fd);
	void						Nick(std::vector<std::string> av, int player_fd);
	void						User(std::vector<std::string> av, int player_fd);
	void						Join(std::vector<std::string> av, int player_fd);
	void						Part(std::vector<std::string> av, int player_fd);
	void						Privmsg(std::vector<std::string> av, int player_fd, std::string str);
	void						Mode(std::vector<std::string> av, int player_fd);
	void						Topic(std::vector<std::string> av, int player_fd);
	void						Invite(std::vector<std::string> av, int player_fd);
	void						Kick(std::vector<std::string> av, int player_fd);
	void						Quit(std::vector<std::string> av, int player_fd);
	void						Exit(std::vector<std::string>, int);
	void						Log(std::vector<std::string>, int);
	// ChannelResult				Names(std::vector<std::string> av, int player_fd);
	// ChannelResult				List(std::vector<std::string> av, int player_fd);
	// ChannelResult				Version(std::vector<std::string> av, int player_fd);
	// ChannelResult				Time(std::vector<std::string> av, int player_fd);
	// ChannelResult				Server(std::vector<std::string> av, int player_fd);
	// ChannelResult				Oper(std::vector<std::string> av, int player_fd);
	// ChannelResult				Squit(std::vector<std::string> av, int player_fd);
	// ChannelResult				Stats(std::vector<std::string> av, int player_fd);
	// ChannelResult				Links(std::vector<std::string> av, int player_fd);
	// ChannelResult				Connect(std::vector<std::string> av, int player_fd);
	// ChannelResult				Trace(std::vector<std::string> av, int player_fd);
	// ChannelResult				Admin(std::vector<std::string> av, int player_fd);
	// ChannelResult				Info(std::vector<std::string> av, int player_fd);
	// ChannelResult				Privmsg(std::vector<std::string> av, int player_fd);
	// ChannelResult				Notice(std::vector<std::string> av, int player_fd);
	// ChannelResult				Who(std::vector<std::string> av, int player_fd);
	// ChannelResult				Whois(std::vector<std::string> av, int player_fd);
	// ChannelResult				Whowas(std::vector<std::string> av, int player_fd);
	// ChannelResult				Kill(std::vector<std::string> av, int player_fd);
	// ChannelResult				Ping(std::vector<std::string> av, int player_fd);
	// ChannelResult				Pong(std::vector<std::string> av, int player_fd);
	// ChannelResult				Error(std::vector<std::string> av, int player_fd);
	// ChannelResult				Away(std::vector<std::string> av, int player_fd);
	// ChannelResult				Rehash(std::vector<std::string> av, int player_fd);
	// ChannelResult				Restart(std::vector<std::string> av, int player_fd);
	// ChannelResult				Summon(std::vector<std::string> av, int player_fd);
	// ChannelResult				Users(std::vector<std::string> av, int player_fd);
	// ChannelResult				Wallops(std::vector<std::string> av, int player_fd);
	// ChannelResult				Userhost(std::vector<std::string> av, int player_fd);
	MessageTranslator();

public:
	MessageTranslator(std::string pass);
	MessageTranslator(const MessageTranslator& other);
	MessageTranslator& operator=(const MessageTranslator& other);
	~MessageTranslator();

	void			Execute(std::string message, int user_fd);
	void			SetOpePass(std::string pass);
	void			OutputLog();
};
