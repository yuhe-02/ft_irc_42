#include "ErrorCode.h"

std::pair<int, std::string> create_code_message(int code, std::string str1, std::string str2, std::string str3)
{
	switch(code) {
		case ERR_NOSUCHNICK:
			return std::make_pair(code, str1 + " :No such nick/channel");
		case ERR_NOSUCHSERVER:
			return std::make_pair(code, str1 + " :No such server");
		case ERR_NOSUCHCHANNEL:
			return std::make_pair(code, str1 + " :No such channel");
		case ERR_CANNOTSENDTOCHAN:
			return std::make_pair(code, str1 + " :Cannot send to channel");
		case ERR_TOOMANYCHANNELS:
			return std::make_pair(code, str1 + " :You have joined too many channels");
		case ERR_WASNOSUCHNICK:
			return std::make_pair(code, str1 + " :There was no such nickname");
		case ERR_TOOMANYTARGETS:
			return std::make_pair(code, str1 + " :Duplicate recipients. No message delivered");
		case ERR_NOORIGIN:
			return std::make_pair(code, ":No origin specified");
		case ERR_NORECIPIENT:
			return std::make_pair(code, ":No recipient given (" + str1 + ")");
		case ERR_NOTEXTTOSEND:
			return std::make_pair(code, ":No text to send");
		case ERR_UNKNOWNCOMMAND:
			return std::make_pair(code, str1 + " :Unknown command");
		case ERR_NOMOTD:
			return std::make_pair(code, ":MOTD File is missing");
		case ERR_NOADMININFO:
			return std::make_pair(code, str1 + " :No administrative info available");
		case ERR_FILEERROR:
			return std::make_pair(code, ":File error doing " + str1 + " on " + str2);
		case ERR_NONICKNAMEGIVEN:
			return std::make_pair(code, ":No nickname given");
		case ERR_ERRONEUSNICKNAME:
			return std::make_pair(code, str1 + " :Erroneous nickname");
		case ERR_NICKNAMEINUSE:
			return std::make_pair(code, str1 + " :Nickname is already in use");
		case ERR_NICKCOLLISION:
			return std::make_pair(code, str1 + " :Nickname collision KILL");
		case ERR_USERNOTINCHANNEL:
			return std::make_pair(code, str1 + " " + str2 + " :They aren't on that channel");
		case ERR_NOTONCHANNEL:
			return std::make_pair(code, str1 + " :You're not on that channel");
		case ERR_USERONCHANNEL:
			return std::make_pair(code, str1 + " " + str2 + " :is already on channel");
		case ERR_NOLOGIN:
			return std::make_pair(code, str1 + " :User not logged in");
		case ERR_SUMMONDISABLED:
			return std::make_pair(code, ":SUMMON has been disabled");
		case ERR_USERSDISABLED:
			return std::make_pair(code, ":USERS has been disabled");
		case ERR_NOTREGISTERED:
			return std::make_pair(code, ":You have not registered");
		case ERR_NEEDMOREPARAMS:
			return std::make_pair(code, str1 + " :Not enough parameters");
		case ERR_ALREADYREGISTRED:
			return std::make_pair(code, ":You may not reregister");
		case ERR_PASSWDMISMATCH:
			return std::make_pair(code, ":Password incorrect");
		case ERR_YOUREBANNEDCREEP:
			return std::make_pair(code, ":You are banned from this server");
		case ERR_KEYSET:
			return std::make_pair(code, str1 + " :Channel key already set");
		case ERR_CHANNELISFULL:
			return std::make_pair(code, str1 + " :Cannot join channel (+l)");
		case ERR_INVITEONLYCHAN:
			return std::make_pair(code, str1 + " :Cannot join channel (+i)");
		case ERR_BANNEDFROMCHAN:
			return std::make_pair(code, str1 + " :Cannot join channel (+b)");
		case ERR_BADCHANNELKEY:
			return std::make_pair(code, str1 + " :Cannot join channel (+k)");
		case ERR_NOPRIVILEGES:
			return std::make_pair(code, ":Permission Denied - You're not an IRC operator");
		case ERR_CHANOPRIVSNEEDED:
			return std::make_pair(code, str1 + " :You're not channel operator");
		case ERR_CANTKILLSERVER:
			return std::make_pair(code, ":You can't kill a server!");
		case ERR_NOOPERHOST:
			return std::make_pair(code, ":No O-lines for your host");
		case ERR_UMODEUNKNOWNFLAG:
			return std::make_pair(code, ":Unknown MODE flag");
		case ERR_USERSDONTMATCH:
			return std::make_pair(code, ":Can't change mode for other users");

		case RPL_NONE:
			return std::make_pair(code, "");
		case RPL_USERHOST:
			return std::make_pair(code, ":" + str1);
		case RPL_ISON:
			return std::make_pair(code, ":" + str1);
		case RPL_AWAY:
			return std::make_pair(code, str1 + " :" + str2);
		case RPL_UNAWAY:
			return std::make_pair(code, ":You are no longer marked as being away");
		case RPL_NOWAWAY:
			return std::make_pair(code, ":You have been marked as being away");
		case RPL_WHOISUSER:
			return std::make_pair(code, str1 + " " + str2 + " " + str3 + " * :" + str3);
		case RPL_WHOISSERVER:
			return std::make_pair(code, str1 + " " + str2 + " :" + str3);
		case RPL_WHOISOPERATOR:
			return std::make_pair(code, str1 + " :is an IRC operator");
		case RPL_WHOISIDLE:
			return std::make_pair(code, str1 + " " + str2 + " :seconds idle");
		case RPL_ENDOFWHOIS:
			return std::make_pair(code, str1 + " :End of /WHOIS list");
		case RPL_WHOISCHANNELS:
			return std::make_pair(code, str1 + " :" + str2);
		case RPL_WHOWASUSER:
			return std::make_pair(code, str1 + " " + str2 + " " + str3 + " * :" + str3);
		case RPL_ENDOFWHOWAS:
			return std::make_pair(code, str1 + " :End of WHOWAS");
		case RPL_LISTSTART:
			return std::make_pair(code, "Channel :Users Name");
		case RPL_LIST:
			return std::make_pair(code, str1 + " " + str2 + " :" + str3);
		case RPL_LISTEND:
			return std::make_pair(code, ":End of /LIST");
		case RPL_CHANNELMODEIS:
			return std::make_pair(code, str1 + " " + str2 + " " + str3);
		default:
			return std::make_pair(code, ":Unknown response code");
	}
}

bool	IsNotError(std::pair<int, std::string> v)
{
	if (v.first > 1)
		return (false);
	return (true);
}

bool	is_special(char c)
{
	if (c == '-' || c ==  '[' || c ==  ']' || c ==  '\\' || c ==  '`' || c ==  '^' || c ==  '{' || c ==  '}')
		return (true);
	return (false);
}

bool	is_letter(char c)
{
	if ((c >='a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return (true);
	return (false);
}

bool	is_number(char c)
{
	if (c >= '0' && c <= '9')
		return (true);
	return (false);
}

bool	is_nick(std::string str)
{
	std::string::iterator ite = str.begin();
	if (!is_letter(*ite))
		return (false);
	for (; ite != str.end(); ite++)
	{
		if (!is_letter(*ite) && !is_special(*ite) && !is_number(*ite))
			return (false);
	}
	return (true);
}
