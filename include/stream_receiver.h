#ifndef __STREAM_RECEIVER_H
#define __STREAM_RECEIVER_H

#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <list>



class stream_receiver_impl;


struct http_output_detect_box {
	int id;
	int box_x;
	int box_y;
	int box_w;
	int box_h;
	int x;
	int y;
	int d;
	int xcm;
	int ycm;
	int zcm;
	int xtcm;
	int ytcm;
	int ztcm;
	float xa;
	float ya;
	float r;
	
};

struct gyro_status {
	float roll;
	float pitch;
};



class stream_receiver
{
public:
	stream_receiver(std::string ip, int port, int stream_number);
	~stream_receiver();
	int query_frame(int timeout = 5);
	int query_frame(std::vector<unsigned char> &image, int timeout = 5);
	
	void get_frame(std::vector<unsigned char> &image);
	void get_detect_boxes(vector<struct http_output_detect_box> &detect_boxes);
	void get_gyro_angle(struct gyro_status &gyro_angle);
	
private:
	stream_receiver_impl *impl_;
	
	
};












#endif

