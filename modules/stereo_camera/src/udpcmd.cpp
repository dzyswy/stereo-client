#include "udpcmd.h"

#include "udpsend.h"



udpcmd::udpcmd(const char* ip, int port)
{
	ip_ = ip;
	port_ = port;
}

int udpcmd::send_cmd(const char* value)
{
	return udpsend(ip_.c_str(), port_, value);
}









