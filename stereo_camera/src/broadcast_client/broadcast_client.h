#ifndef __BROADCAST_CLIENT_H
#define __BROADCAST_CLIENT_H


#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <ctime>



class broadcast_receiver;






class broadcast_client
{
public:
	broadcast_client(const char *device_name, int port, int poll_time);
	~broadcast_client();
	void get_device_nodes(std::map<std::string, std::map<std::string, std::string> > &device_nodes);

protected:	
	broadcast_receiver *impl_;
};








#endif





