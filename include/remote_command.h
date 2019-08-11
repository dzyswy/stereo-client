#ifndef __REMOTE_COMMAND_H
#define __REMOTE_COMMAND_H


#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <mutex>




namespace rpc {
	class client;
	
}


class remote_command
{
public:
	remote_command(const char *ip, int port);
	
	int set_value(const char *cmd, int value, int timeout = 5000);
	int set_value(const char *cmd, float value, int timeout = 5000);
	int set_value(const char *cmd, std::string value, int timeout = 5000);
	
	int get_value(const char *cmd, int &value, int timeout = 5000);
	int get_value(const char *cmd, float &value, int timeout = 5000);
	int get_value(const char *cmd, std::string &value, int timeout = 5000);
	
	

protected:
	std::string ip_;
	int port_;
	
};





#endif

