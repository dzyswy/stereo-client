#ifndef __STEREO_STRUCT_H
#define __STEREO_STRUCT_H


#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>


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
	int pan;
	int tilt;
	int zoom;
};

struct stereo_gyro_angle
{
	float xabs;
	float yabs;
	float zabs;
	float roll;
	float pitch;
};


enum PTZ_CHANNEL_TYPE
{
	PTZ_PAN_CHANNEL = 0,
	PTZ_TILT_CHANNEL = 1,
	PTZ_ZOOM_CHANNEL = 2,
	PTZ_MAX_CHANNEL = 3,
}

struct stereo_ptz_pose
{
	int val[PTZ_MAX_CHANNEL];
};







#endif


