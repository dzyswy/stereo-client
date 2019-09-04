#ifndef __STEREO_CAMERA_H
#define __STEREO_CAMERA_H




#include "stereo_detect_boxes.h"
#include "stereo_gyro_angle.h"



class command_client;
class stream_receiver;



class stereo_camera
{
public:
	stereo_camera();
	~stereo_camera();
	int open_device(const char *ip, int cmd_port, int stream_port, int stream_index);
	int close_device();
	
	int set_value(const char *cmd, int value, int timeout = 5000);
	int set_value(const char *cmd, float value, int timeout = 5000);
	int set_value(const char *cmd, std::string value, int timeout = 5000);
	int set_poly_mask(std::vector<std::pair<float, float> > &value, int timeout = 5000);
	
	int get_value(const char *cmd, int &value, int timeout = 5000);
	int get_value(const char *cmd, float &value, int timeout = 5000);
	int get_value(const char *cmd, std::string &value, int timeout = 5000);
	int get_poly_mask(std::vector<std::pair<float, float> > &value, int timeout = 5000);
	
	
	int get_pixel_point(int x, int y, struct stereo_pixel_point &value, int timeout = 5000);
	
	
	int query_frame(int timeout = 5);
	void get_frame(std::vector<unsigned char> &image);
	void get_detect_boxes(std::vector<struct stereo_detect_box> &detect_boxes);
	void get_gyro_angle(struct stereo_gyro_angle &gyro_angle);
	int get_reconnect_count();
	
protected:
	std::string ip_;
	int stream_port_;
	int stream_mode_;
	int cmd_port_;
	
	command_client *xcmd_;
	stream_receiver *xstream_;
	
};
















#endif




