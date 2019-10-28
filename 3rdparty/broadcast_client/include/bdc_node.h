#ifndef __BDC_NODE_H
#define __BDC_NODE_H


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




class bdc_node
{
public:
	bdc_node(std::string ip, time_t deadline, std::map<std::string, std::string> &headers)
	{
		ip_ = ip;
		deadline_ = deadline;
		headers_ = headers;
	}
	
	void set_ip(std::string value)
	{
		ip_ = value;
	}
	
	void set_deadline(time_t value)
	{
		deadline_ = value;
	}
	
	void set_headers(std::map<std::string, std::string> &headers)
	{
		headers_ = headers;
	}
	
	std::string get_ip()
	{
		return ip_;
	}
	
	time_t get_deadline()
	{
		return deadline_;
	}
	
	std::map<std::string, std::string> get_headers()
	{
		return headers_;
	}
	
protected:
	std::string ip_;
	time_t deadline_;
	std::map<std::string, std::string> headers_;
	
};




class bdc_nodes
{
public:
	bdc_nodes();
	~bdc_nodes();
	
	void add_node(std::string ip, time_t deadline, std::map<std::string, std::string> &headers);
	void delete_node();
	
	void get_nodes(std::map<std::string, bdc_node*> &value);

protected:
	std::mutex mux_;
	std::map<std::string, bdc_node*> nodes_;
	
};








#endif


