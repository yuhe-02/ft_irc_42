#include "../includes/SocketServer.hpp"

void SocketServer::setNonBlocking(int fd) {
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
bool SocketServer::initServer() {
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
    setNonBlocking(server_fd_);

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


void SocketServer::handleNewConnection() {
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_fd = accept(server_fd_, (struct sockaddr *)&client_addr, &client_len);
	struct pollfd pfd;

	if (client_fd < 0) {
		perror("accept");
		return;
	}
	setNonBlocking(client_fd);
	pfd.fd = client_fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	poll_fds_.push_back(pfd);

	std::cout << "New client connected: " << inet_ntoa(client_addr.sin_addr)
			  << ":" << ntohs(client_addr.sin_port) << "\n";
}

/**
 * @brief クライアントからのメッセージ受信
 *
 * @return 成功時はmessage、失敗時は "" を返す
 */
std::string SocketServer::receiveMessage(int fd) {
    std::string message;
    int bytesRead;
    char buffer[BUFFER_SIZE + 1];

    while (true) {
        memset(buffer, 0, BUFFER_SIZE + 1);
        bytesRead = recv(fd, buffer, BUFFER_SIZE, 0);
        if (bytesRead < 0) {
            // 途中送信
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                break;
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
    }
    std::cout << "Received: " << message;
    return message;
}

/**
 * @brief クライアントからのメッセージ処理
 *
 * クライアントからのメッセージを処理する。ircのコマンドに応じて処理を行う。
 *
 */
void SocketServer::handleClientMessage(size_t index) {
    int client_fd = poll_fds_[index].fd;
    std::string message = receiveMessage(client_fd);

    // クライアント切断処理
    if (message.empty()) {
        message_buffer_[client_fd].clear();
        closeClient(index);
        return;
    }
    // バッファに追記
    message_buffer_[client_fd] += message;
    std::string &buffer = message_buffer_[client_fd];
    size_t pos;
    // 終端記号（\r\n または \n）検出時に処理
    while ((pos = buffer.find('\n')) != std::string::npos) {
        std::string line = buffer.substr(0, pos);  // 1行分抽出
        buffer.erase(0, pos + 1);                  // バッファ更新
        // \r削除（Windows互換用）
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line.erase(line.size() - 1, 1);
        }
        translator_->Execute(line, client_fd);
    }
}

void SocketServer::closeClient(size_t index)
{
	std::cout << "Client disconnected: FD " << poll_fds_[index].fd << "\n";
	close(poll_fds_[index].fd);
	poll_fds_.erase(poll_fds_.begin() + index);
}

SocketServer::SocketServer(int port, const std::string &password) : port_(port), password_(password), server_fd_(-1), translator_(new MessageTranslator(password)){
    everyone_ = Everyone::GetInstance();
}

SocketServer::~SocketServer() {
	cleanUp();
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
void SocketServer::start() {
	if (!initServer()) {
		return;
	}
	while (true) {
        std::vector<int> indices_to_remove;
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
					handleNewConnection();
				} else {
					handleClientMessage(i);
                    // Everyoneクラスでユーザーが作成されないと即時切断するようにしてるけどどうしようかな
                    if ((message_buffer_[poll_fds_[i].fd].size() == 0) && !(everyone_->IsCreated(poll_fds_[i].fd))) {
                        indices_to_remove.push_back(i);
                    }
				}
			}

		}
        for (std::vector<int>::reverse_iterator it = indices_to_remove.rbegin();
            it != indices_to_remove.rend(); ++it) {
            std::cout << "Client disconnected: FD " << poll_fds_[*it].fd << "\n";
            close(poll_fds_[*it].fd);
            poll_fds_.erase(poll_fds_.begin() + *it);
        }
	}
}

void SocketServer::cleanUp() {
	for (size_t i = 0; i < poll_fds_.size(); i++) {
		close(poll_fds_[i].fd);
	}
	std::cout << "Server shutdown.\n";
}
