#ifndef __CAMERA_COMMAND_H
#define __CAMERA_COMMAND_H


#include "remote_command.h"



struct point_space_status {
	int x;
	int y;
	int d;
	int xcm;
	int ycm;
	int zcm;
	int xtcm;
	int ytcm;
	int ztcm;
	double xa;
	double ya;
	double r;
};



class camera_command : public remote_command
{
public:
	camera_command(const char *ip, int port);
	int set_poly_mask(std::vector<std::pair<int, int> > &value, int timeout = 5000);
	int get_disparity(int x, int y, int &d, int timeout = 5000);
	int get_point_space_status(int x, int y, struct point_space_status &status, int timeout = 5000);
//	int get_poly_mask(std::vector<std::pair<int, int> > &value, int timeout = 5000);
};












#endif

