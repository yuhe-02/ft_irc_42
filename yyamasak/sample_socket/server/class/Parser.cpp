#include "../includes/Parser.hpp"

IntrusivePtr<Parser> Parser::instance_ = IntrusivePtr<Parser>(0);

Parser::Parser() : RefCounted() {}

Parser::Parser(const Parser &) : RefCounted() {}

Parser &Parser::operator=(const Parser &)
{
	return (*this);
}

Parser::~Parser()
{
}

IntrusivePtr<Parser> Parser::GetInstance()
{
	if (!instance_)
	{
		instance_ = IntrusivePtr<Parser>(new Parser());
	}
	return instance_;
}

/**
 * @brief クライアントの登録
 *
 * クライアントがニックネームとユーザー名を登録しているか確認し、登録されていればクライアントを登録する。
 */
void Parser::registerClient(int client_fd)
{
	ClientInfo &client = clients_[client_fd];
	if (client.hasNick && client.hasUser && !client.registered)
	{
		client.registered = true;
		client.isLoggedIn = true;
		std::string server_name = "ft_irc";
		std::string nick = client.nick;
		std::string welcome_msg = ":" + server_name + " 001 " + nick + " :Welcome to the ft_irc server!\r\n";
		send(client_fd, welcome_msg.c_str(), welcome_msg.length(), 0);
		std::string your_host_msg = ":" + server_name + " 002 " + nick + " :Your host is " + server_name + ", running version 1.0\r\n";
		send(client_fd, your_host_msg.c_str(), your_host_msg.length(), 0);
		std::string created_msg = ":" + server_name + " 003 " + nick + " :This server was created today\r\n";
		send(client_fd, created_msg.c_str(), created_msg.length(), 0);
		std::string myinfo_msg = ":" + server_name + " 004 " + nick + " " + server_name + " 1.0 o o\r\n";
		send(client_fd, myinfo_msg.c_str(), myinfo_msg.length(), 0);

		std::cout << "Registered client: " << client.nick << std::endl;
	}
}

void Parser::action(const std::string &line, int client_fd, const std::string &password)
{
	std::string new_nick;
	std::pair<int, std::string> response;
	bool nick_in_use;

	ClientInfo &client = clients_[client_fd];
	std::cout << "Processing command: " << line << std::endl;
	if (line.compare(0, 4, "PASS") == 0)
	{
		// TODO: PASS処理
		if (line.substr(5) == password)
		{
			client.isLoggedIn = true;
		}
		else
		{
			response = create_code_message(464, "Password incorrect");
			send(client_fd, response.second.c_str(), response.first, 0);
		}
	}
	// CAP LS 応答
	// サーバー上で利用可能な機能をクライアントに通知する
	else if (line.compare(0, 6, "CAP LS") == 0)
	{
		std::string capabilities = ":multi-prefix sasl";
		std::string response_msg = ": ft_irc CAP * LS :" + capabilities + "\r\n";
	
		response = std::pair<int, std::string>(response_msg.length(), response_msg);
		send(client_fd, response.second.c_str(), response.second.length(), 0);
	}	
	// CAP REQ 応答
	// クライアントがサーバーに対して要求する機能を通知する
	else if (line.compare(0, 7, "CAP REQ") == 0)
	{
		std::string requested_cap = line.substr(8); // "CAP REQ "の後ろを取得
		requested_cap.erase(requested_cap.find_last_not_of("\r\n") + 1); // 末尾の改行削除
		std::string response_msg = ": ft_irc CAP * ACK :" + requested_cap + "\r\n";
		response = std::pair<int, std::string>(response_msg.length(), response_msg);
		send(client_fd, response.second.c_str(), response.second.length(), 0);
	}
	// CAP END 応答なし (交渉終了のため)
	else if (line.compare(0, 7, "CAP END") == 0)
	{
		// 何もしない
	}
	// NICK 処理
	// クライアントのニックネームを設定する(受け取ったニックネームがすでに使われている場合はエラーを返す)
	else if (line.compare(0, 4, "NICK") == 0)
	{
		new_nick = line.substr(5);
		nick_in_use = false;
		for (std::map<int, ClientInfo>::iterator it = clients_.begin(); it != clients_.end(); ++it)
		{
			if (it->second.nick == new_nick && it->first != client_fd)
			{
				nick_in_use = true;
				break;
			}
		}
		if (nick_in_use)
		{
			response = std::pair<int, std::string>(433, ":ft_irc 433 *" + new_nick + " :Nickname is already in use\r\n");
			send(client_fd, response.second.c_str(), response.second.length(), 0);
		}
		else
		{
			client.nick = new_nick;
			client.hasNick = true;
			registerClient(client_fd);
		}
	}
	// USER 処理
	// クライアントのユーザー名を設定する(nickとuserが設定されている場合にクライアントを登録する)
	else if (line.compare(0, 4, "USER") == 0)
	{
		client.user = line.substr(5);
		client.hasUser = true;
		registerClient(client_fd);
	}
	// PING → PONG 応答
	// クライアントからのPINGメッセージに対してPONGメッセージを返す
	// PINGメッセージは、クライアントがサーバーに対して接続が有効であることを確認するために使用される
	// 断続的にクライアントから送られてくる
	else if (line.compare(0, 4, "PING") == 0)
	{
		response = std::pair<int, std::string>(1, ":ft_irc PONG : PING\r\n");
		send(client_fd, response.second.c_str(), response.second.length(), 0);
	}
	else if (line.compare(0, 4, "JOIN") == 0)
	{
		// TODO: JOIN処理
	}
	else if (line.compare(0, 7, "PRIVMSG") == 0)
	{
		// TODO: PRIVMSG処理
	}
	else if (line.compare(0, 4, "QUIT") == 0)
	{
		// TODO: QUIT処理
		clients_.erase(client_fd);
	}
}

bool Parser::isExist(int client_fd)
{
	return clients_.find(client_fd) != clients_.end();
}