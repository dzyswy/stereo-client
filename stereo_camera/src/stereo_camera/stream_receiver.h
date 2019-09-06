#ifndef __STREAM_RECEIVER_H
#define __STREAM_RECEIVER_H

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

#include "stereo_detect_boxes.h"
#include "stereo_gyro_angle.h"


class stream_receiver_impl;



class stream_receiver
{
public:
	stream_receiver(int debug = 0);
	~stream_receiver();
	int connect_stream(const char *ip, int port, int index);
	int disconnect_stream();
	
	int query_frame(int timeout = 5);
	void get_frame(std::vector<unsigned char> &image);
	void get_detect_boxes(std::vector<struct stereo_detect_box> &detect_boxes);
	void get_gyro_angle(struct stereo_gyro_angle &gyro_angle);
	int get_reconnect_count();
	
protected:
	void stream_process();
	
private:
	std::string ip_;
	int port_;
	int index_;
	
	int reconnect_count_;
	
	std::vector<unsigned char> frame_buffer_;
	struct stereo_detect_boxes detect_boxes_;
	struct stereo_gyro_angle gyro_angle_;

	int going;
	std::mutex mux_;
	std::condition_variable cond_;
	std::thread *run_thread_;
	
	int debug_;
	
};












#endif

