#ifndef __UDP_CMD_UDP_CMD_H
#define __UDP_CMD_UDP_CMD_H

#include <iostream>
#include <stdio.h>

class udpcmd
{
public:
	udpcmd(const char* ip, int port);
	int send_cmd(const char* value);

protected:
	std::string ip_;
	int port_;
};










#endif

