#ifndef __STEREO_STRUCT_H
#define __STEREO_STRUCT_H


#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>



struct stereo_pixel_point
{
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



struct stereo_detect_box
{
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

struct stereo_gyro_angle
{
	float xabs;
	float yabs;
	float zabs;
	float roll;
	float pitch;
};











#endif


