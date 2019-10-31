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

#include "bdc_node.h"


class broadcast_receiver;


struct broadcast_node
{
	std::string ip;
	std::string board;
	std::string company;
	std::string version;
	std::string serial_number;
};



class broadcast_client
{
public:
	broadcast_client(const char *device_name, int port, int poll_time, int debug = 0);
	~broadcast_client();
	void run();
	void stop();
	void get_device_nodes(std::map<std::string, bdc_node*> &device_nodes);

protected:	
	broadcast_receiver *impl_;
	bdc_nodes nodes_;
};








#endif





