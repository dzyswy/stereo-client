#ifndef __STEREO_STREAM_H
#define __STEREO_STREAM_H


#include "stream_receiver.h"



class stereo_stream()
{
public:
	stereo_stream();
	~stereo_stream();
	int connect_stream(const char *ip, int port);
	int disconnect_stream();
	int query_frame(int timeout = 5);
	void get_frame(std::vector<unsigned char> &image);
	void get_detect_boxes(vector<struct http_output_detect_box> &detect_boxes);
	void get_gyro_angle(struct gyro_status &gyro_angle);
	
protected:
	
	
	std::string ip_;
	int port_;
	
	std::vector<unsigned char> frame_buffer_;
	vector<struct http_output_detect_box> detect_boxes_;
	struct gyro_status gyro_angle_;

	int going;
	std::mutex mux_;
	std::condition_variable cond_;
	std::thread *run_thread_;
};












#endif


