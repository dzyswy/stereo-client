#ifndef __COMMAND_CLIENT_H
#define __COMMAND_CLIENT_H


#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <mutex>


#include "stereo_pixel_point.h"


namespace rpc {
	class client;
	
}


class command_client
{
public:
	void set_connect(const char *ip, int port);
	
	int set_value(const char *para, int value, int timeout = 5000);
	int set_value(const char *para, float value, int timeout = 5000);
	int set_value(const char *para, std::string value, int timeout = 5000);
	int set_poly_mask(std::vector<std::pair<float, float> > &value, int timeout = 5000);
	
	int get_value(const char *para, int &value, int timeout = 5000);
	int get_value(const char *para, float &value, int timeout = 5000);
	int get_value(const char *para, std::string &value, int timeout = 5000);
	int get_poly_mask(std::vector<std::pair<float, float> > &value, int timeout = 5000);
	

	int get_pixel_point(int x, int y, struct stereo_pixel_point &value, int timeout = 5000);
	
	int do_action(const char *para, int timeout = 5000);


protected:
	std::string ip_;
	int port_;
	
};





#endif

