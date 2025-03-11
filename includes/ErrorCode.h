#pragma once

#include <iostream>
#include <string>
#include <utility>


enum IrcErrorCode {
	FATAL = -9999,
	ERR_NOSUCHNICK = 401,       // No such nick/channel
	ERR_NOSUCHSERVER = 402,     // No such server
	ERR_NOSUCHCHANNEL = 403,    // No such channel
	ERR_CANNOTSENDTOCHAN = 404, // Cannot send to channel
	ERR_TOOMANYCHANNELS = 405,  // You have joined too many channels
	ERR_WASNOSUCHNICK = 406,    // There was no such nickname
	ERR_TOOMANYTARGETS = 407,   // Duplicate recipients. No message delivered
	ERR_NOORIGIN = 409,         // No origin specified
	ERR_NORECIPIENT = 411,      // No recipient given (<command>)
	ERR_NOTEXTTOSEND = 412,     // No text to send
	ERR_NOTOPLEVEL = 413,       // No toplevel domain specified
	ERR_WILDTOPLEVEL = 414,     // Wildcard in toplevel domain
	ERR_UNKNOWNCOMMAND = 421,   // Unknown command
	ERR_NOMOTD = 422,           // MOTD File is missing
	ERR_NOADMININFO = 423,      // No administrative info available
	ERR_FILEERROR = 424,        // File error doing <file op> on <file>
	ERR_NONICKNAMEGIVEN = 431,  // No nickname given
	ERR_ERRONEUSNICKNAME = 432, // Erroneous nickname
	ERR_NICKNAMEINUSE = 433,    // Nickname is already in use
	ERR_NICKCOLLISION = 436,    // Nickname collision KILL
	ERR_USERNOTINCHANNEL = 441, // They aren't on that channel
	ERR_NOTONCHANNEL = 442,     // You're not on that channel
	ERR_USERONCHANNEL = 443,    // is already on channel
	ERR_NOLOGIN = 444,          // User not logged in
	ERR_SUMMONDISABLED = 445,   // SUMMON has been disabled
	ERR_USERSDISABLED = 446,    // USERS has been disabled
	ERR_NOTREGISTERED = 451,    // You have not registered
	ERR_NEEDMOREPARAMS = 461,   // Not enough parameters
	ERR_ALREADYREGISTRED = 462, // You may not reregister
	ERR_NOPERMFORHOST = 463,    // Your host isn't among the privileged
	ERR_PASSWDMISMATCH = 464,   // Password incorrect
	ERR_YOUREBANNEDCREEP = 465, // You are banned from this server
	ERR_KEYSET = 467,           // Channel key already set
	ERR_CHANNELISFULL = 471,    // Cannot join channel (+l)
	ERR_UNKNOWNMODE = 472,      // is unknown mode char to me
	ERR_INVITEONLYCHAN = 473,   // Cannot join channel (+i)
	ERR_BANNEDFROMCHAN = 474,   // Cannot join channel (+b)
	ERR_BADCHANNELKEY = 475,    // Cannot join channel (+k)
	ERR_NOPRIVILEGES = 481,     // Permission Denied- You're not an IRC operator
	ERR_CHANOPRIVSNEEDED = 482, // You're not channel operator
	ERR_CANTKILLSERVER = 483,   // You cant kill a server!
	ERR_NOOPERHOST = 491,       // No O-lines for your host
	ERR_UMODEUNKNOWNFLAG = 501, // Unknown MODE flag
	ERR_USERSDONTMATCH = 502    // Cant change mode for other users
};

enum IrcResponseCode {
	RPL_WELCOME = 1,            // Welcome to the Internet Relay Network
	RPL_NONE = 300,             // Dummy reply number. Not used
	RPL_USERHOST = 302,         // Reply to USERHOST command
	RPL_ISON = 303,             // Reply to ISON command
	RPL_AWAY = 301,             // Away message
	RPL_UNAWAY = 305,           // You are no longer marked as being away
	RPL_NOWAWAY = 306,          // You have been marked as being away
	RPL_WHOISUSER = 311,        // WHOIS user details
	RPL_WHOISSERVER = 312,      // WHOIS server details
	RPL_WHOISOPERATOR = 313,    // WHOIS: user is an IRC operator
	RPL_WHOISIDLE = 317,        // WHOIS idle time
	RPL_ENDOFWHOIS = 318,       // End of WHOIS list
	RPL_WHOISCHANNELS = 319,    // WHOIS channels list
	RPL_WHOWASUSER = 314,       // WHOWAS user details
	RPL_ENDOFWHOWAS = 369,      // End of WHOWAS list
	RPL_LISTSTART = 321,        // Start of LIST command
	RPL_LIST = 322,             // Channel list
	RPL_LISTEND = 323,          // End of LIST command
	RPL_CHANNELMODEIS = 324,    // Channel mode details
	RPL_NOTOPIC = 331,          // No topic is set
	RPL_TOPIC = 332,            // Channel topic
	RPL_INVITING = 341,         // Invitation sent
	RPL_SUMMONING = 342,        // Summoning user to IRC
	RPL_VERSION = 351,          // Server version details
	RPL_WHOREPLY = 352,         // WHO reply
	RPL_ENDOFWHO = 315,         // End of WHO list
	RPL_NAMREPLY = 353,         // NAMES reply
	RPL_ENDOFNAMES = 366,       // End of NAMES list
	RPL_LINKS = 364,            // LINKS list
	RPL_ENDOFLINKS = 365,       // End of LINKS list
	RPL_BANLIST = 367,          // Active ban list
	RPL_ENDOFBANLIST = 368,     // End of ban list
	RPL_INFO = 371,             // INFO message
	RPL_ENDOFINFO = 374,        // End of INFO list
	RPL_MOTDSTART = 375,        // Start of MOTD
	RPL_MOTD = 372,             // MOTD text
	RPL_ENDOFMOTD = 376,        // End of MOTD command
	RPL_YOUREOPER = 381,        // You are now an IRC operator
	RPL_REHASHING = 382,        // Rehashing configuration
	RPL_TIME = 391,             // Server local time
	RPL_USERSSTART = 392,       // Start of USERS list
	RPL_USERS = 393,            // USERS list
	RPL_ENDOFUSERS = 394,       // End of USERS list
	RPL_NOUSERS = 395,          // Nobody logged in
	RPL_TRACELINK = 200,        // TRACE link
	RPL_TRACECONNECTING = 201,  // TRACE connecting
	RPL_TRACEHANDSHAKE = 202,   // TRACE handshake
	RPL_TRACEUNKNOWN = 203,     // TRACE unknown
	RPL_TRACEOPERATOR = 204,    // TRACE operator
	RPL_TRACEUSER = 205,        // TRACE user
	RPL_TRACESERVER = 206,      // TRACE server
	RPL_TRACENEWTYPE = 208,     // TRACE new type
	RPL_TRACELOG = 261,         // TRACE log
	RPL_STATSLINKINFO = 211,    // STATS link info
	RPL_STATSCOMMANDS = 212,    // STATS commands
	RPL_STATSCLINE = 213,       // STATS C-line
	RPL_STATSNLINE = 214,       // STATS N-line
	RPL_STATSILINE = 215,       // STATS I-line
	RPL_STATSKLINE = 216,       // STATS K-line
	RPL_STATSYLINE = 218,       // STATS Y-line
	RPL_ENDOFSTATS = 219,       // End of STATS report
	RPL_STATSLLINE = 241,       // STATS L-line
	RPL_STATSUPTIME = 242,      // Server uptime
	RPL_STATSOLINE = 243,       // STATS O-line
	RPL_STATSHLINE = 244,       // STATS H-line
	RPL_UMODEIS = 221,          // User mode string
	RPL_LUSERCLIENT = 251,      // LUSER client info
	RPL_LUSEROP = 252,          // LUSER operators online
	RPL_LUSERUNKNOWN = 253,     // LUSER unknown connections
	RPL_LUSERCHANNELS = 254,    // LUSER channels formed
	RPL_LUSERME = 255,          // LUSER server info
	RPL_ADMINME = 256,          // Admin info (server)
	RPL_ADMINLOC1 = 257,        // Admin info (location 1)
	RPL_ADMINLOC2 = 258,        // Admin info (location 2)
	RPL_ADMINEMAIL = 259        // Admin info (email)
};

enum IrcReservedCode {
	RPL_TRACECLASS = 209,        // TRACE class
	RPL_STATSQLINE = 217,        // STATS Q-line
	RPL_SERVICEINFO = 231,       // Service information
	RPL_ENDOFSERVICES = 232,     // End of services list
	RPL_SERVICE = 233,           // Service
	RPL_SERVLIST = 234,          // Service list
	RPL_SERVLISTEND = 235,       // End of service list
	RPL_WHOISCHANOP = 316,       // WHOIS channel operator
	RPL_KILLDONE = 361,          // KILL command completed
	RPL_CLOSING = 362,           // Closing link
	RPL_CLOSEEND = 363,          // End of closing links
	RPL_INFOSTART = 373,         // Start of INFO
	RPL_MYPORTIS = 384,          // Your port is...
	ERR_YOUWILLBEBANNED = 466,   // You will be banned
	ERR_BADCHANMASK = 476,       // Bad channel mask
	ERR_NOSERVICEHOST = 492      // No service host
};

typedef std::pair<int, std::string> ChannelResult;

ChannelResult create_code_message(int code, std::string str1 = "", std::string str2 = "", std::string str3 = "", std::string str4 = "");
bool						is_special(char c);
bool						is_letter(char c);
bool						is_number(char c);
bool						is_nick(std::string str);
