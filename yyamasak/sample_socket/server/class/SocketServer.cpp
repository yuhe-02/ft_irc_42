#include "../includes/SocketServer.hpp"

void SocketServer::SetNonBlocking(int fd) {
	fcntl(fd, F_SETFL, O_NONBLOCK);
}


/**
 * @brief ソケットサーバーの初期化
 *
 * 1. ソケットを作成する。
 * 2. ソケットにIPアドレスとポート番号をバインドする。
 * 3. クライアントからの接続要求を待機する。
 *
 * @return 成功時は true、失敗時は false
 */
bool SocketServer::InitServer() {
    /**
     * @brief ソケットの作成
     *
     * `socket()` 関数を使用してソケットを作成し、そのファイルディスクリプタを取得する。
     * 引数:
     *   - `AF_INET` : IPv4アドレスファミリー
     *   - `SOCK_STREAM` : TCPストリーム型
     *   - `0` : プロトコルを自動選択（通常はTCP）
     */
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ == -1) {
        perror("socket");
        return false;
    }

    // ソケットオプション設定（アドレス再利用を許可）
    int opt = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // サーバーアドレス構造体の初期化
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_);

    /**
     * @brief ソケットの関連付け
     *
     * `bind()` 関数を使用してソケットに IP アドレスやポート番号の設定をおこなう。
     * 引数:
     *   - `server_fd_` : 対象ソケットのfd
     *   - `server_addr` : ソケットに割り当てるアドレスやポートの情報
     *   - `sizeof(server_addr)` : addrのサイズ
     */
    if (bind(server_fd_, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        return false;
    }

    /**
     * @brief ソケットの関連付け
     *
     * `listen()` 関数を使用してクライアントから接続要求がくるまで、サーバー側で「接続待ち」をおこない、待機させる。
     * 引数:
     *   - `server_fd_` : 対象ソケットのfd
     *   - `MAX_CLIENTS` : 接続要求を保持する数 -> accept していない接続要求を、最大いくつまで溜めておくかを指定する。
     *                     これにより、サーバーは接続要求が来たタイミングではなく、サーバー自身のタイミングが良いときに接続を受けつけ、
     *                     実際のデータのやり取りを行うようなことが可能。
     */
    if (listen(server_fd_, MAX_CLIENTS) == -1) {
        perror("listen");
        return false;
    }

    // ノンブロッキングモードを設定
    SetNonBlocking(server_fd_);

    /**
     * @brief pfdの初期化、サーバー側に接続要求が来たときに検知する変数の設定(検知を有効化しているわけではない)
     * 
     * pollで、fdを監視してイベントを通知してくれる
     * pfdメンバ変数:
     *   - `server_fd_`: 監視するfd
     *   - `POLLIN`: データ受信可能な状態 (新しい接続要求があるか) を監視
     *   - `0`: 初期値として 0 をセット（poll 実行後にイベントが入る）
     */
    struct pollfd pfd;
    pfd.fd = server_fd_;
    pfd.events = POLLIN;
    pfd.revents = 0;
    poll_fds_.push_back(pfd);

    // サーバーの起動メッセージを表示
    std::cout << "Server listening on port " << port_ << "...\n";

    return true;
}


void SocketServer::HandleNewConnection() {
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_fd = accept(server_fd_, (struct sockaddr *)&client_addr, &client_len);
	struct pollfd pfd;

	if (client_fd < 0) {
		perror("accept");
		return;
	}
	SetNonBlocking(client_fd);
	pfd.fd = client_fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	poll_fds_.push_back(pfd);
    auth_map_[client_fd] = false;

	std::cout << "New client connected: " << inet_ntoa(client_addr.sin_addr)
			  << ":" << ntohs(client_addr.sin_port) << "\n";
}

std::string SocketServer::receiveMessage(int fd) {
    std::string message;
    int bytesRead;
    char buffer[BUFFER_SIZE + 1];

    while (true) {
        memset(buffer, 0, BUFFER_SIZE + 1);  
        bytesRead = recv(fd, buffer, BUFFER_SIZE, 0);
        if (bytesRead < 0) {
            // 単にデータ送信がないだけの場合
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                continue;
            } else {
                perror("recv");
                break;
            }
        } else if (bytesRead == 0) {
            break;
        }
        buffer[bytesRead] = '\0';
        message.append(buffer);
        if (message.find('\n') != std::string::npos) {
            break;
        }
        std::cout << "Received: " << message << std::endl;
    }
    return message;
}

void SocketServer::registerClient(int client_fd) {
    ClientInfo &client = clients[client_fd];
    if (client.hasNick && client.hasUser && !client.registered) {
        client.registered = true;

        // Welcome メッセージ送信
        std::string welcome = ":server 001 " + client.nick + " :Welcome to the IRC Network\r\n";
        send(client_fd, welcome.c_str(), welcome.size(), 0);

        std::cout << "Registered client: " << client.nick << std::endl;
    }
}

void SocketServer::HandleClientMessage(size_t index) {
    int client_fd = poll_fds_[index].fd;
    std::string message = receiveMessage(client_fd);
    std::istringstream stream(message);
    std::string line;
    std::string new_nick;
    std::string response;
    bool nick_in_use;
    
    if (message.empty()) {
        CloseClient(index);
        return;
    }
    std::cout << "Client [" << client_fd << "] says: " << message << std::endl;
    while (std::getline(stream, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line.erase(line.size() - 1, 1);
        }
        ClientInfo &client = clients[client_fd];
        std::cout << "Processing command: " << line << std::endl;
        // CAP LS 応答
        if (line.compare(0, 6, "CAP LS") == 0) {
            response = ":server CAP * LS :multi-prefix sasl\r\n";
            send(client_fd, response.c_str(), response.size(), 0);
        }
        // CAP REQ 応答
        else if (line.compare(0, 7, "CAP REQ") == 0) {
            response = ":server CAP * ACK :" + line.substr(8) + "\r\n";
            send(client_fd, response.c_str(), response.size(), 0);
        }
        // CAP END 応答なし (交渉終了のため)
        else if (line.compare(0, 7, "CAP END") == 0) {
            // 何もしない
        }
        // NICK 処理
        else if (line.compare(0, 4, "NICK") == 0) {
            new_nick = line.substr(5);
            nick_in_use = false;
            for (std::map<int, ClientInfo>::iterator it = clients.begin(); it != clients.end(); ++it) {
                if (it->second.nick == new_nick && it->first != client_fd) {
                    nick_in_use = true;
                    break;
                }
            }
            if (nick_in_use) {
                response = ":server 433 * " + new_nick + " :Nickname is already in use\r\n";
                send(client_fd, response.c_str(), response.size(), 0);
            } else {
                client.nick = new_nick;
                client.hasNick = true;
                registerClient(client_fd);
            }
        }
        // USER 処理
        else if (line.compare(0, 4, "USER") == 0) {
            client.user = line.substr(5);
            client.hasUser = true;
            registerClient(client_fd);
        }
        // PING → PONG 応答
        else if (line.compare(0, 4, "PING") == 0) {
            response = ":server PONG" + line.substr(4) + "\r\n";
            send(client_fd, response.c_str(), response.size(), 0);
        }
    }
}

void SocketServer::CloseClient(size_t index)
{
	std::cout << "Client disconnected: FD " << poll_fds_[index].fd << "\n";
	close(poll_fds_[index].fd);
	poll_fds_.erase(poll_fds_.begin() + index);
}

SocketServer::SocketServer(int port, const std::string &password) : port_(port), password_(password), server_fd_(-1) {}

SocketServer::~SocketServer() {
	CleanUp();
}

/**
 * @brief 実行部分
 *
 * 1. サーバー設定
 * 2. イベントを監視する
 *   2-1. 接続要求なら接続関数呼び出し
 *   2-2. クライアントからの受信なら受信関数呼び出し
 * 3. 何か起きたら終了
 *
 */
void SocketServer::Start() {
	if (!InitServer()) {
		return;
	}
	while (true) {
        /**
         * @brief 監視設定
         *
         * `poll()` 関数を使用して監視設定を行う
         * 引数:
         *   - `poll_fds_.data()`: 監視対象のpollfdの配列の先頭ポインタ
         *   - `poll_fds_.size()`: 監視対象の個数
         *   - `-1`: timeout
        */
		int ret = poll(poll_fds_.data(), poll_fds_.size(), -1);
		if (ret < 0) {
			perror("poll");
			break;
		}
		for (size_t i = 0; i < poll_fds_.size(); i++) {
            // reventsには、複数のビットフラグが同時に入る可能性がある(POLLIN以外にも入っている可能性がある)
            // なのでAND演算で、その場合でも対応できるようにしている。
			if (poll_fds_[i].revents & POLLIN) {
				if (poll_fds_[i].fd == server_fd_) {
					HandleNewConnection();
				} else {
					HandleClientMessage(i);
				}
			}
		}
	}
}

void SocketServer::CleanUp() {
	for (size_t i = 0; i < poll_fds_.size(); i++) {
		close(poll_fds_[i].fd);
	}
	std::cout << "Server shutdown.\n";
}
