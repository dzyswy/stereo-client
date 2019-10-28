#ifndef __RPC_RECEIVER_H
#define __RPC_RECEIVER_H


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


using namespace asio;
using namespace asio::ip; 



class rpc_receiver
{
public:
	rpc_receiver(std::string ip, int port, std::string request, int debug = 0);
	~rpc_receiver();
	int get_result(std::string &result, int timeout = 5);
	
protected:
	void do_connect(ip::tcp::endpoint &ep);
	void do_write_request();
	void do_read_status_line();
	void do_read_headers();
	
	void set_header(std::map<std::string, std::string> &headers, const std::string key, std::string &value);
	int get_header(std::map<std::string, std::string> &headers, const std::string key, std::string &value);
	
protected:
	asio::io_context io_context_;
	tcp::socket socket_;
	asio::streambuf request_;
	asio::streambuf response_;
	
	int debug_;
	std::string request_s_;
	
	int ret_;
	std::string result_;

	std::mutex mux_;
	std::condition_variable cond_;
	std::thread *run_thread_;
};















#endif


