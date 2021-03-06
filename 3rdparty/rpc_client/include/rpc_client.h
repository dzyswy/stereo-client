#ifndef __RPC_CLIENT_H
#define __RPC_CLIENT_H

#include <iostream>
#include <sstream>
#include <functional>
#include <vector>
#include <map>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>


class rpc_client
{
public:	
	rpc_client(int debug = 0);
	int set_connect(const char *ip, int port);
	
	int set_value(const std::string key, int value, int timeout = 5);
	int set_value(const std::string key, float value, int timeout = 5);
	int set_value(const std::string key, std::string value, int timeout = 5);
	
	int get_value(const std::string key, int &value, int timeout = 5);
	int get_value(const std::string key, float &value, int timeout = 5);
	int get_value(const std::string key, std::string &value, int timeout = 5);
	int get_value(const std::string key, const std::string &para, std::string &value, int timeout = 5);
	
	int do_action(const std::string key, int timeout = 5);
	
protected:	
	std::string to_string(int value)
	{
		std::stringstream os;
		os << value;
		return os.str();
	}
	
	std::string to_string(float value)
	{
		std::stringstream os;
		os << value;
		return os.str();
	}
	
	std::string to_string(double value)
	{
		std::stringstream os;
		os << value;
		return os.str();
	}
	
	int check_ip(const char *value);
	
protected:	
	std::string ip_;
	int port_;
	int debug_;
};






#endif


