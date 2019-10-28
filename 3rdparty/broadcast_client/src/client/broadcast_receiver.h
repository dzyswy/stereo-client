#ifndef __DISCOVERY_RECEIVER_H
#define __DISCOVERY_RECEIVER_H

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
#include <asio.hpp>
#include <asio/steady_timer.hpp>

#include "bdc_node.h"


using namespace asio;
using namespace asio::ip;






class broadcast_receiver
{
public:
	broadcast_receiver(const char *device_name, int port, int poll_time, bdc_nodes& nodes, int debug = 0);
	~broadcast_receiver();
	
	void get_device_nodes(std::map<std::string, struct bdc_dev_node> &device_nodes);

protected:	
	void do_receive();
	void add_node(std::string dev_ip, int max_age);
	void do_timer();
	
	 
	void set_header(std::map<std::string, std::string> &headers, const std::string key, std::string &value);
	int get_header(std::map<std::string, std::string> &headers, const std::string key, std::string &value); 
	

protected:	
	asio::io_context io_context_;
	asio::ip::udp::socket socket_;
	asio::ip::udp::endpoint sender_endpoint_;
	asio::steady_timer timer_;
	
	enum { max_length = 10240 };
	char data_[max_length];
	
	int debug_;
	std::string device_name_;
	int poll_time_;
	
	
    bdc_nodes &device_nodes_;
	
	std::thread *run_thread_;	
};









#endif

