#ifndef __FIT_CALIB_SAMPLES_H
#define __FIT_CALIB_SAMPLES_H


#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <exception>
#include <string>
#include <vector>






enum fit_calib_coord_type
{
	FIT_CALIB_GRAPH_COORD = 0,
	FIT_CALIB_CAMEAR_COORD = 1,
	FIT_CALIB_ROOM_COORD = 2,
	FIT_CALIB_BALL_COORD = 3,
	FIT_CALIB_MAX_COORD = 4,
};

enum fit_calib_ptz_channel_type
{
	FIT_CALIB_PTZ_PAN = 0,
	FIT_CALIB_PTZ_TILT = 1,
	FIT_CALIB_PTZ_ZOOM = 2,
	FIT_CALIB_PTZ_MAX_CHANNEL = 3,
};

struct fit_calib_pose_sample
{
	float val[FIT_CALIB_PTZ_MAX_CHANNEL];
};

struct fit_calib_pixel_sample
{
	float val[FIT_CALIB_MAX_COORD][FIT_CALIB_PTZ_MAX_CHANNEL];
};


struct fit_calib_samples
{
	std::vector<std::pair<struct fit_calib_pose_sample, struct fit_calib_pixel_sample> > samples;
	
	int to_string(std::string &value);
	int from_string(std::string &value);
	
};

















#endif


