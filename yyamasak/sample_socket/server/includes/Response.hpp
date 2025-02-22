#ifndef __RESPONSE_HPP__
# define __RESPONSE_HPP__

# include <iostream>
# include <string>
# include <utility>

class Response {
	public:
		Response();
		~Response();
		static std::pair<int, std::string> getNumberResponse(int error_code, const std::string& property);
		static std::pair<int, std::string> getResponse(const std::string &kinds, const std::string &message);
		static std::string getWelcomeResponse(const std::string &nick);
};
#endif