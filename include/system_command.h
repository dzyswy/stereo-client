#ifndef __SYSTEM_COMMAND_H
#define __SYSTEM_COMMAND_H



#include "remote_command.h"


class system_command : public remote_command
{
public:
	system_command(const char *ip, int port);
	int download_update_system(std::string host, std::string file, std::string user, std::string passwd, int force, int timeout = 5000);
};


















#endif

