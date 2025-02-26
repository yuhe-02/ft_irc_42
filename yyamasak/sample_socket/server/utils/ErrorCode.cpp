#include "../includes/ErrorCode.h"

pair<int, std::string> create_code_message(int code, string str1, string str2, string str3) {
    switch(code) {
        // --- エラーコード ---
        case ERR_NOSUCHNICK:
            return make_pair(code, str1 + " :No such nick/channel");
        case ERR_NOSUCHSERVER:
            return make_pair(code, str1 + " :No such server");
        case ERR_NOSUCHCHANNEL:
            return make_pair(code, str1 + " :No such channel");
        case ERR_CANNOTSENDTOCHAN:
            return make_pair(code, str1 + " :Cannot send to channel");
        case ERR_TOOMANYCHANNELS:
            return make_pair(code, str1 + " :You have joined too many channels");
        case ERR_WASNOSUCHNICK:
            return make_pair(code, str1 + " :There was no such nickname");
        case ERR_TOOMANYTARGETS:
            return make_pair(code, str1 + " :Duplicate recipients. No message delivered");
        case ERR_NOORIGIN:
            return make_pair(code, ":No origin specified");
        case ERR_NORECIPIENT:
            return make_pair(code, ":No recipient given (" + str1 + ")");
        case ERR_NOTEXTTOSEND:
            return make_pair(code, ":No text to send");
        case ERR_UNKNOWNCOMMAND:
            return make_pair(code, str1 + " :Unknown command");
        case ERR_NOMOTD:
            return make_pair(code, ":MOTD File is missing");
        case ERR_NOADMININFO:
            return make_pair(code, str1 + " :No administrative info available");
        case ERR_FILEERROR:
            return make_pair(code, ":File error doing " + str1 + " on " + str2);
        case ERR_NONICKNAMEGIVEN:
            return make_pair(code, ":No nickname given");
        case ERR_ERRONEUSNICKNAME:
            return make_pair(code, str1 + " :Erroneous nickname");
        case ERR_NICKNAMEINUSE:
            return make_pair(code, str1 + " :Nickname is already in use");
        case ERR_NICKCOLLISION:
            return make_pair(code, str1 + " :Nickname collision KILL");
        case ERR_USERNOTINCHANNEL:
            return make_pair(code, str1 + " " + str2 + " :They aren't on that channel");
        case ERR_NOTONCHANNEL:
            return make_pair(code, str1 + " :You're not on that channel");
        case ERR_USERONCHANNEL:
            return make_pair(code, str1 + " " + str2 + " :is already on channel");
        case ERR_NOLOGIN:
            return make_pair(code, str1 + " :User not logged in");
        case ERR_SUMMONDISABLED:
            return make_pair(code, ":SUMMON has been disabled");
        case ERR_USERSDISABLED:
            return make_pair(code, ":USERS has been disabled");
        case ERR_NOTREGISTERED:
            return make_pair(code, ":You have not registered");
        case ERR_NEEDMOREPARAMS:
            return make_pair(code, str1 + " :Not enough parameters");
        case ERR_ALREADYREGISTRED:
            return make_pair(code, ":You may not reregister");
        case ERR_PASSWDMISMATCH:
            return make_pair(code, ":Password incorrect");
        case ERR_YOUREBANNEDCREEP:
            return make_pair(code, ":You are banned from this server");
        case ERR_KEYSET:
            return make_pair(code, str1 + " :Channel key already set");
        case ERR_CHANNELISFULL:
            return make_pair(code, str1 + " :Cannot join channel (+l)");
        case ERR_INVITEONLYCHAN:
            return make_pair(code, str1 + " :Cannot join channel (+i)");
        case ERR_BANNEDFROMCHAN:
            return make_pair(code, str1 + " :Cannot join channel (+b)");
        case ERR_BADCHANNELKEY:
            return make_pair(code, str1 + " :Cannot join channel (+k)");
        case ERR_NOPRIVILEGES:
            return make_pair(code, ":Permission Denied - You're not an IRC operator");
        case ERR_CHANOPRIVSNEEDED:
            return make_pair(code, str1 + " :You're not channel operator");
        case ERR_CANTKILLSERVER:
            return make_pair(code, ":You can't kill a server!");
        case ERR_NOOPERHOST:
            return make_pair(code, ":No O-lines for your host");
        case ERR_UMODEUNKNOWNFLAG:
            return make_pair(code, ":Unknown MODE flag");
        case ERR_USERSDONTMATCH:
            return make_pair(code, ":Can't change mode for other users");

        // --- レスポンスコード ---
        case RPL_NONE:
            return make_pair(code, "");
        case RPL_USERHOST:
            return make_pair(code, ":" + str1);
        case RPL_ISON:
            return make_pair(code, ":" + str1);
        case RPL_AWAY:
            return make_pair(code, str1 + " :" + str2);
        case RPL_UNAWAY:
            return make_pair(code, ":You are no longer marked as being away");
        case RPL_NOWAWAY:
            return make_pair(code, ":You have been marked as being away");
        case RPL_WHOISUSER:
            return make_pair(code, str1 + " " + str2 + " " + str3 + " * :" + str3);
        case RPL_WHOISSERVER:
            return make_pair(code, str1 + " " + str2 + " :" + str3);
        case RPL_WHOISOPERATOR:
            return make_pair(code, str1 + " :is an IRC operator");
        case RPL_WHOISIDLE:
            return make_pair(code, str1 + " " + str2 + " :seconds idle");
        case RPL_ENDOFWHOIS:
            return make_pair(code, str1 + " :End of /WHOIS list");
        case RPL_WHOISCHANNELS:
            return make_pair(code, str1 + " :" + str2);
        case RPL_WHOWASUSER:
            return make_pair(code, str1 + " " + str2 + " " + str3 + " * :" + str3);
        case RPL_ENDOFWHOWAS:
            return make_pair(code, str1 + " :End of WHOWAS");
        case RPL_LISTSTART:
            return make_pair(code, "Channel :Users Name");
        case RPL_LIST:
            return make_pair(code, str1 + " " + str2 + " :" + str3);
        case RPL_LISTEND:
            return make_pair(code, ":End of /LIST");
        case RPL_CHANNELMODEIS:
            return make_pair(code, str1 + " " + str2 + " " + str3);
        default:
            return make_pair(code, ":Unknown response code");
    }
}