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


enum STEREO_COORD_TYPE
{
	STEREO_GRAPH_COORD = 0,
	STEREO_DEPTH_COORD = 1,
	STEREO_CAMERA_COORD = 2,
	STEREO_ROOM_COORD = 3,
	STEREO_BALL_COORD = 4,
	STEREO_PTZ_COORD = 5,
	STEREO_MAX_COORD = 6,
};

#define STEREO_GRAPH_COORD_MASK		(1 << 0)
#define STEREO_DEPTH_COORD_MASK		(1 << 1)
#define STEREO_CAMERA_COORD_MASK	(1 << 2)
#define STEREO_ROOM_COORD_MASK		(1 << 3)
#define STEREO_BALL_COORD_MASK		(1 << 4)
#define STEREO_PTZ_COORD_MASK		(1 << 5)
#define STEREO_ALL_COORD_MASK		(0xffffffff)



struct stereo_detect_box
{
	int id;
	//二维坐标系
	int box_x;
	int box_y;
	int box_w;
	int box_h;
	//视差坐标系
	int x;
	int y;
	int d;
	//摄像机坐标系
	int xcm;
	int ycm;
	int zcm;
	//教室坐标系
	int xtcm;
	int ytcm;
	int ztcm;
	//球坐标系
	float xa;
	float ya;
	float r;
	//PTZ坐标系
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
};

#define PTZ_PAN_MASK		(1 << 0)
#define PTZ_TILT_MASK		(1 << 1)
#define PTZ_ZOOM_MASK		(1 << 2)
#define PTZ_ALL_MASK		(0xffffffff)


struct stereo_ptz_pose
{
	int val[PTZ_MAX_CHANNEL];
};







#endif


