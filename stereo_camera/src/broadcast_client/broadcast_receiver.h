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


using namespace asio;
using namespace asio::ip;



struct bdc_dev_node
{
	time_t timeout;
	std::map<std::string, std::string> headers;
};


class broadcast_receiver
{
public:
	broadcast_receiver(const char *device_name, int port, int poll_time);
	~broadcast_receiver();
	
	void get_device_nodes(std::map<std::string, struct bdc_dev_node> &device_nodes);

protected:	
	void do_receive();
	void add_node(std::string dev_ip, int max_age);
	void do_timer();
	
	void erase_timeout_device_node();
	void set_device_node(std::string ip, time_t timeout, std::map<std::string, std::string> &headers);
	int get_header(std::map<std::string, std::string> &headers, const std::string key, std::string &value);
	void clear_device_nodes();
	

protected:	
	asio::io_context io_context_;
	asio::ip::udp::socket socket_;
	asio::ip::udp::endpoint sender_endpoint_;
	asio::steady_timer timer_;
	
	enum { max_length = 1024 };
	char data_[max_length];
	
	std::string device_name_;
	int poll_time_;
	
	std::mutex mux_;
    std::map<std::string, struct bdc_dev_node> device_nodes_;
	
	std::thread *run_thread_;	
};









#endif

