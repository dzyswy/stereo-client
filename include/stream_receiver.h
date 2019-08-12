#ifndef __STREAM_RECEIVER_H
#define __STREAM_RECEIVER_H

#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <list>



class stream_receiver_impl;


class stream_receiver
{
public:
	stream_receiver(std::string ip, int port, int stream_number);
	~stream_receiver();
	int query_frame(std::vector<unsigned char> &image, int timeout = 5);
	
private:
	stream_receiver_impl *impl_;
	
	
};












#endif

