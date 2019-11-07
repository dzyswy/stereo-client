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

#include "stereo_struct.h"



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

enum STEREO_CAMERA_POST_GRAY_MODE_TYPE
{
	STEREO_CAMERA_POST_GRAY_CLOSE_MODE = 0,
	STEREO_CAMERA_POST_GRAY_OPEN_MODE = 1,
};


class rpc_client;
class stream_client;

 

class stereo_camera
{
public:
	stereo_camera(int debug = 0);
	~stereo_camera();

	int open_device(const char *ip, int cmd_port);
	int open_device(const char *ip, int cmd_port, int stream_port, int stream_index);
	int close_device();
	int is_opened();
	
	int set_value(const char *key, int value, int timeout = 5);
	int set_value(const char *key, float value, int timeout = 5);
	int set_value(const char *key, std::string value, int timeout = 5);
	int set_poly_mask(std::vector<std::pair<float, float> > &value, int timeout = 5);
	int set_ptz_samples(std::vector<std::pair<struct stereo_ptz_pose, struct stereo_detect_box> > &samples, int timeout = 5);
	
	int get_value(const char *key, int &value, int timeout = 5);
	int get_value(const char *key, float &value, int timeout = 5);
	int get_value(const char *key, std::string &value, int timeout = 5);
	int get_value(const char *key, const std::string &para, std::string &value, int timeout = 5);
	int get_poly_mask(std::vector<std::pair<float, float> > &value, int timeout = 5);
	
	
	int get_detect_point(int x, int y, struct stereo_detect_box &value, int timeout = 5);
	int do_action(const char *para, int timeout = 5);
	
	int query_frame(int timeout = 5);
	void get_image(std::vector<unsigned char> &image);
	int get_frame_count(int &frame_count);
	int get_detect_boxes(std::vector<struct stereo_detect_box> &detect_boxes);
	int get_gyro_angle(struct stereo_gyro_angle &gyro_angle);
	int get_reconnect_count();
	

protected:
	int open_;
	
	std::mutex mux_;

	rpc_client *xcmd_;
	stream_client *xstream_;
		
	
};



















#endif
