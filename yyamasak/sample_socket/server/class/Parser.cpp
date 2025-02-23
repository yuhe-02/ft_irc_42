#include "../includes/Parser.hpp"

Parser::Parser() : RefCounted(){}

Parser::Parser(const Parser &) : RefCounted(){}

Parser &Parser::operator=(const Parser &)
{
	return (*this);
}

Parser::~Parser()
{

}

IntrusivePtr<Parser>	Parser::GetInstance()
{
	if (!instance_) {
		instance_ = IntrusivePtr<Parser>(new Parser());
	}
	return instance_;
}

void Parser::action(const std::string &str)
{
	ClientInfo &client = clients_[client_fd];
	std::cout << "Processing command: " << line << std::endl;
	if (line.compare(0, 4, "PASS") == 0) {
		// TODO: PASS処理
		if (line.substr(5) == password_) {
			auth_map_[client_fd] = true;
		} else {
			response = Response::getNumberResponse(464, "Password incorrect");
			send(client_fd, response.second.c_str(), response.first, 0);
		}
	}
	// if (auth_map_[client_fd] == false) {
	//     response = ":server 451 * :You have not registered\r\n";
	//     send(client_fd, response.c_str(), response.size(), 0);
	//     continue;
	// } 
	// CAP LS 応答
	// サーバー上で利用可能な機能をクライアントに通知する
	else if (line.compare(0, 6, "CAP LS") == 0) {
		response = Response::getResponse("CAP * LS", "multi-prefix sasl");
		send(client_fd, response.second.c_str(), response.first, 0);
	}
	// CAP REQ 応答
	// クライアントがサーバーに対して要求する機能を通知する
	else if (line.compare(0, 7, "CAP REQ") == 0) {
		response = Response::getResponse("CAP * ACK", line.substr(8));
		send(client_fd, response.second.c_str(), response.first, 0);
	}
	// CAP END 応答なし (交渉終了のため)
	else if (line.compare(0, 7, "CAP END") == 0) {
		// 何もしない
	}
	// NICK 処理
	// クライアントのニックネームを設定する(受け取ったニックネームがすでに使われている場合はエラーを返す)
	else if (line.compare(0, 4, "NICK") == 0) {
		new_nick = line.substr(5);
		nick_in_use = false;
		for (std::map<int, ClientInfo>::iterator it = clients_.begin(); it != clients_.end(); ++it) {
			if (it->second.nick == new_nick && it->first != client_fd) {
				nick_in_use = true;
				break;
			}
		}
		if (nick_in_use) {
			response = Response::getNumberResponse(433, new_nick);
			send(client_fd, response.second.c_str(), response.first, 0);
		} else {
			client.nick = new_nick;
			client.hasNick = true;
			registerClient(client_fd);
		}
	}
	// USER 処理
	// クライアントのユーザー名を設定する(nickとuserが設定されている場合にクライアントを登録する)
	else if (line.compare(0, 4, "USER") == 0) {
		client.user = line.substr(5);
		client.hasUser = true;
		registerClient(client_fd);
	}
	// PING → PONG 応答
	// クライアントからのPINGメッセージに対してPONGメッセージを返す
	// PINGメッセージは、クライアントがサーバーに対して接続が有効であることを確認するために使用される
	// 断続的にクライアントから送られてくる
	else if (line.compare(0, 4, "PING") == 0) {
		response = Response::getResponse("PONG", line.substr(4));
		send(client_fd, response.second.c_str(), response.first, 0);
	}
	else if (line.compare(0, 4, "JOIN") == 0) {
		// TODO: JOIN処理
	}
	else if (line.compare(0, 7, "PRIVMSG") == 0) {
		// TODO: PRIVMSG処理
	}
	else if (line.compare(0, 4, "QUIT") == 0) {
		// TODO: QUIT処理
		closeClient(index);
	}
}