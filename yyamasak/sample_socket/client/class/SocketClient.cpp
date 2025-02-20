#include "../includes/SocketClient.hpp"

SocketClient::SocketClient(const std::string &ip, int port) : server_ip(ip), port(port), client_socket(-1) {}

SocketClient::~SocketClient()
{
	if (client_socket != -1)
	{
		close(client_socket);
		std::cout << "Socket closed.\n";
	}
}

bool SocketClient::connectToServer()
{
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1)
	{
		perror("socket");
		return false;
	}

	struct sockaddr_in server_addr;
	std::memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0)
	{
		perror("inet_pton");
		return false;
	}

	if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("connect");
		return false;
	}

	std::cout << "Connected to the server!\n";
	return true;
}

bool SocketClient::sendMessage(const std::string &message)
{
	if (send(client_socket, message.c_str(), message.length(), 0) == -1)
	{
		perror("send");
		return false;
	}
	return true;
}

std::string SocketClient::receiveMessage()
{
	char buffer[BUFFER_SIZE];
	std::memset(buffer, 0, BUFFER_SIZE);

	int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
	if (bytes_received <= 0)
	{
		return "";
	}
	return std::string(buffer);
}

void SocketClient::run()
{
	if (!connectToServer())
	{
		return;
	}

	std::string message;
	while (true)
	{
		std::cout << "Send message > ";
		std::getline(std::cin, message);
		message.append("\n");

		if (message == "exit")
		{
			break;
		}

		if (!sendMessage(message))
		{
			break;
		}

		std::string response = receiveMessage();
		if (response.empty())
		{
			std::cout << "Server closed the connection.\n";
			break;
		}

		std::cout << "Server response: " << response << std::endl;
	}
}
