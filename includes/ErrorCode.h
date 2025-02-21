#pragma once

#include <iostream>

enum ErrorCode
{
	TRUE = 1,
	UNKNOWN = -9999,
	NO_USER = -1000,
	NO_CHANNEL,
	ALREADY_JOINED,
	ALREADY_LEAVE,
	NEED_PASSWORD,
	OUT_OF_RANGE,
	NO_AUTHORITY,
	LAST_OPERATOR,
	NO_JOINED
};

template<typename Type>
void	printerror(int error_code, Type message)
{
	std::cerr << "Error: ";
	switch (error_code)
	{
	case NO_USER:
		std::cerr << "No User " << message << std::endl;
		break;
	case NO_CHANNEL:
		std::cerr << "No Channel " << message << std::endl;
		break;
	case ALREADY_JOINED:
		std::cerr << "already joined " << message << std::endl;
		break;
	case ALREADY_LEAVE:
		std::cerr << "already leave " << message << std::endl;
		break;
	case NEED_PASSWORD:
		std::cerr << "need password " << message << std::endl;
		break;
	case OUT_OF_RANGE:
		std::cerr << "out of range " << message << std::endl;
		break;
	case NO_AUTHORITY:
		std::cerr << "no authority " << message << std::endl;
		break;
	case LAST_OPERATOR:
		std::cerr << "last operator " << message << std::endl;
		break;
	case NO_JOINED:
		std::cerr << "not joined " << message << std::endl;
		break;
	default:
		break;
	}
}
