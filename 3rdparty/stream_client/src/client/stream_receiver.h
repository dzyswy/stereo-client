#ifndef __STREAM_RECEIVER_H
#define __STREAM_RECEIVER_H


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





#define BOUNDARYSTRING "--BOUNDARYSTRING\r\n"



using namespace asio;
using namespace asio::ip;


class stream_receiver
{
public:	
	stream_receiver(std::string ip, int port, int index, int debug = 0);
	~stream_receiver();
	
	int query_frame(int timeout = 5);
	void get_image(std::vector<unsigned char> &image);
	int get_header(const std::string key, std::string &value);
	std::map<std::string, std::string> get_headers();
	
protected:
	void do_connect(ip::tcp::endpoint &ep);
	void do_write();
	void do_read();
	void do_boundary(int flag, int frame_size = 0);
	void do_headers();
	void do_content(int frame_size); 
	
protected:
	asio::io_context io_context_;
	tcp::socket socket_;
	asio::streambuf request_;
	asio::streambuf response_;
	
	int debug_;

	
	
	std::vector<unsigned char> image_;
	std::map<std::string, std::string> headers_;
	
	std::mutex mux_;
	std::condition_variable cond_;
	std::thread *run_thread_;	
	
};











#endif

