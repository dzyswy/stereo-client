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
#include <sys/time.h>

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


/*
namespace asio {
	class io_context;
	class steady_timer;
	namespace ip {
		namespace udp {
			class socket;
			class endpoint;
		}
	}
}

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
	void do_receive();
	void add_node(std::string dev_ip, int max_age);
	void do_timer();

protected:	
	asio::io_context *io_context_;
	asio::ip::udp::socket *socket_;
	asio::ip::udp::endpoint *sender_endpoint_;
	asio::steady_timer *timer_;
	
	enum { max_length = 1024 };
	char data_[max_length];
	
	std::string device_name_;
	int poll_time_;
	
	std::mutex mux_;
    std::list<struct device_node> device_nodes_;
	
	std::thread *run_thread_;
};
*/










#endif

