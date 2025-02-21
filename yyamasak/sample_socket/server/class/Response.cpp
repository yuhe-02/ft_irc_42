#include "Response.hpp"

Response::Response() {}
Response::~Response() {}

std::pair<int, std::string> Response::getNumberResponse(int error_code, const std::string& property) {
	if (error_code == 1) {
		return getResponse("001 *", "Welcome to the Internet Relay Network");
	}
	else if (error_code == 464) {
		return getResponse("464 *", "Password incorrect");
	}
	else if (error_code == 433) {
		return getResponse("433 *" + property, "Nickname is already in use");
	}
	else {
		return getResponse("400 *", "Unknown command");
	}
}

std::pair<int, std::string> Response::getResponse(const std::string& kinds, const std::string& message) {
	std::string response = ":ft_irc " + kinds + " :" + message + "\r\n";
	int size = response.size();
	std::cout << "Response: " << response;
	return std::pair<int, std::string>(size, response);
}

std::string Response::getWelcomeResponse(const std::string& nick) {
	return "Welcome, " + nick + "!";
}