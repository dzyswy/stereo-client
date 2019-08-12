#include "stereo_stream.h"






stereo_stream::stereo_stream(const char *ip, int port)
{
	ip_ = ip;
	port = port;
}


int stereo_stream::connect_stream(const char *ip, int port)
{
	std::unique_lock<std::mutex> lock(mux_);
	
	if (stream_)
		return -1;
	
	ip_ = ip;
	port_ = port;
	
	going = 1;

	return 0;
}

int stereo_stream::disconnect_stream()
{
	std::unique_lock<std::mutex> lock(mux_);
	

	going = 0;
	

	return 0;
}

int stereo_stream::query_frame(std::vector<unsigned char> &image, int timeout)
{
	int ret;
	std::unique_lock<std::mutex> lock(mux_);
	

	if (!stream_) {
		stream_ =  new stream_receiver(ip_, port_);
	}
		
	ret = stream_->query_frame(image, timeout);
	if (ret < 0) {
		delete stream_;
		return -1;
	}
	
	
	
	return 0;
}












