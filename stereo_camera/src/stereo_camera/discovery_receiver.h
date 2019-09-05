#ifndef __DISCOVERY_RECEIVER_H
#define __DISCOVERY_RECEIVER_H

#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <ctime>

class discovery_receiver_impl;

struct device_node {
	std::string ip;
	time_t timeout;
};

class discovery_receiver
{
public:
	discovery_receiver(const char *device_name, int port, int poll_time);
	~discovery_receiver();
	void get_device_nodes(std::vector<std::string> &device_nodes);

protected:	
	discovery_receiver_impl *impl_;
};











#endif

