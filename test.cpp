#include <string>
#include <sstream>
#include <iostream>

int main(){
	std::string str("hello world my new world");
	std::string tmp;
	std::stringstream ss2(str);
	ss2 >> tmp;
	ss2 >> tmp;
	std::cout << 	str.substr(str.find(' ', str.find(' ') + 1) + 1) << std::endl;

}
