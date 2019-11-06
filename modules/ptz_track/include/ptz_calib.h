#ifndef __PTZ_CALIB_H
#define __PTZ_CALIB_H

#include "stereo_struct.h"


class poly_fit;

enum PTZ_CALIB_COORD_TYPE
{
	FIT_CALIB_GRAPH_COORD = 0,
	FIT_CALIB_DEPTH_COORD = 1,
	FIT_CALIB_CAMEAR_COORD = 2,
	FIT_CALIB_ROOM_COORD = 3,
	FIT_CALIB_BALL_COORD = 4,
	FIT_CALIB_MAX_COORD = 5,
};


class ptz_calib
{
public:
	ptz_calib(int debug);
	~ptz_calib();
	int compute(std::vector<std::pair<struct stereo_ptz_pose, struct stereo_pixel_box> > &samples)
	
	
protected:
	int debug_;
	int coord_;
	int degree_[PTZ_MAX_CHANNEL];
	poly_fit *fits_[PTZ_MAX_CHANNEL];
		
};














#endif

