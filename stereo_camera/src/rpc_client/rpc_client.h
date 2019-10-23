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



class rpc_client
{
public:	
	void set_connect(const char *ip, int port);
	
	int set_value(const std::string para, int value, int timeout = 5);
	int set_value(const std::string para, float value, int timeout = 5);
	int set_value(const std::string para, std::string value, int timeout = 5);
	
	int get_value(const std::string para, int &value, int timeout = 5);
	int get_value(const std::string para, float &value, int timeout = 5);
	int get_value(const std::string para, std::string &value, int timeout = 5);
	int get_value(const std::string para, std::string &value, std::string &result, int timeout = 5);
	
	int do_action(const std::string para, int timeout = 5);
	
protected:	
	std::string ip_;
	int port_;
	
};






#endif


