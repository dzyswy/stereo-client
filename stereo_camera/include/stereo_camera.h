#ifndef __STEREO_CAMERA_H
#define __STEREO_CAMERA_H

#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>


#include "stereo_detect_boxes.h"
#include "stereo_gyro_angle.h"
#include "stereo_pixel_point.h"


class discovery_receiver;
class command_client;
class stream_receiver;



enum STEREO_CAMERA_MATCH_MODE_TYPE
{
	STEREO_CAMERA_MATCH_CLOSE_MODE = 2,
	STEREO_CAMERA_MATCH_OPEN_MODE = 1,
	
};

enum STEREO_CAMERA_BG_MODE_TYPE
{
	STEREO_CAMERA_BG_CLOSE_MODE = 2,
	STEREO_CAMERA_BG_OPEN_MODE = 0,
};

enum STEREO_CAMERA_MEDIAN_MODE_TYPE
{
	STEREO_CAMERA_MEDIAN_CLOSE_MODE = 0,
	STEREO_CAMERA_MEDIAN_OPEN_MODE = 1,
};

enum STEREO_CAMERA_TEX_MODE_TYPE
{
	STEREO_CAMERA_TEX_CLOSE_MODE = 0,
	STEREO_CAMERA_TEX_OPEN_MODE = 1,
};

enum STEREO_CAMERA_SPACE_MODE_TYPE
{
	STEREO_CAMERA_SPACE_CLOSE_MODE = 0,
	STEREO_CAMERA_SPACE_OPEN_MODE = 1,
};


enum STEREO_CAMERA_MORPH_MODE_TYPE
{
	STEREO_CAMERA_MORPH_CLOSE_MODE = 0,
	STEREO_CAMERA_MORPH_OPEN_MODE = 1,
};

enum STEREO_CAMERA_POLY_MODE_TYPE
{
	STEREO_CAMERA_POLY_CLOSE_MODE = 0,
	STEREO_CAMERA_POLY_OPEN_MODE = 1,
};


class stereo_camera
{
public:
	stereo_camera(const char *device_name = "zynq_stereo_camera", int port = 45789, int poll_time = 5, int debug = 0);
	~stereo_camera();
	
	void get_device_nodes(std::vector<std::string> &device_nodes);
	
	
	int open_device(const char *ip, int cmd_port, int stream_port, int stream_index);
	int close_device();
	int is_opened();
	
	int set_value(const char *cmd, int value, int timeout = 5000);
	int set_value(const char *cmd, float value, int timeout = 5000);
	int set_value(const char *cmd, std::string value, int timeout = 5000);
	int set_poly_mask(std::vector<std::pair<float, float> > &value, int timeout = 5000);
	
	int get_value(const char *cmd, int &value, int timeout = 5000);
	int get_value(const char *cmd, float &value, int timeout = 5000);
	int get_value(const char *cmd, std::string &value, int timeout = 5000);
	int get_poly_mask(std::vector<std::pair<float, float> > &value, int timeout = 5000);
	
	
	int get_pixel_point(int x, int y, struct stereo_pixel_point &value, int timeout = 5000);
	int do_action(const char *para, int timeout = 5000);
	
	int query_frame(int timeout = 5);
	void get_frame(std::vector<unsigned char> &image);
	void get_detect_boxes(std::vector<struct stereo_detect_box> &detect_boxes);
	void get_gyro_angle(struct stereo_gyro_angle &gyro_angle);
	int get_reconnect_count();
	
	void detect_box_to_pixel(struct stereo_detect_box &detect_box, struct stereo_pixel_point &detect_pixel);
	void detect_pixel_to_box(struct stereo_pixel_point &detect_pixel, struct stereo_detect_box &detect_box);
	
protected:
	std::string ip_;
	int stream_port_;
	int stream_mode_;
	int cmd_port_;
	int open_;
	
	std::mutex mux_;
	
	discovery_receiver *xfind;
	command_client *xcmd_;
	stream_receiver *xstream_;
	
};
















#endif




