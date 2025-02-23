#ifndef __PARSER_HPP__
# define ___PARSER_HPP__

#include <string>
#include "RefCounted.hpp"
#include "IntrusivePtr.hpp"

class Parser : public RefCounted {
	private:
		static IntrusivePtr<Parser> instance_;
	public:
		Parser();
		Parser(const Parser &other);
		Parser &operator=(const Parser &other);
		~Parser();
		static IntrusivePtr<Parser>	GetInstance();
		void action(const std::string &str);

};
# endif