#ifndef __STREAM_CLIENT_H
#define __STREAM_CLIENT_H



#include <iostream>
#include <sstream>
#include <functional>
#include <vector>
#include <map>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mutex>
#include <thread>
#include <condition_variable>



class stream_client
{
public:
	stream_client(int debug = 0);
	~stream_client();
	
	int connect_stream(const char *ip, int port, int index);
	int disconnect_stream();
	
	int get_reconnect_count();
	
	int query_frame(int timeout = 5);
	void get_image(std::vector<unsigned char> &image);
	int get_header(const std::string key, std::string &value);
	
	
protected:
	void stream_process();
	int check_ip(const char *value);
	
private:
	std::string ip_;
	int port_;
	int index_;
	
	int debug_;
	
	int reconnect_count_;
	
	std::vector<unsigned char> image_;
	std::map<std::string, std::string> headers_;
	

	int going;
	std::mutex lock_;
	std::mutex mux_;
	std::condition_variable cond_;
	std::thread *run_thread_;
	
	
	
};













#endif

