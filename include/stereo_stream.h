#ifndef __STEREO_STREAM_H
#define __STEREO_STREAM_H


#include "stream_receiver.h"



class stereo_stream()
{
public:
	stereo_stream();
	int connect_stream(const char *ip, int port);
	int disconnect_stream();
	int query_frame(std::vector<unsigned char> &image, int timeout = 5);
	
protected:
	stream_receiver *stream_;
	
	std::string ip_;
	int port_;

	int going;
	std::mutex mux_;
	std::thread *run_thread_;
};












#endif


