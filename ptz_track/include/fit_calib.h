#ifndef __FIT_CALIB_H
#define __FIT_CALIB_H





#include "stereo_detect_boxes.h"


class poly_fit;


//sensor -> ptz

enum FIT_CALIB_FIT_MODE_TYPE
{
	FIT_CALIB_CLOSE_TO_MODE = 0,
	FIT_CALIB_IN_LINE_MODE = 1,
	
};

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

struct fit_calib_ptz_pose
{
	float val[FIT_CALIB_PTZ_MAX_CHANNEL];
};

struct fit_calib_detect_pose
{
	float val[FIT_CALIB_PTZ_MAX_CHANNEL];
};



class fit_calib
{
public:
	fit_calib();
	~fit_calib();

	void set_fit_mode(int value);
	int get_fit_mode();
 
	int set_sample(int pan, int tilt, int zoom, struct stereo_detect_box &detect_box, int index);
	int get_sample(struct fit_calib_ptz_pose &ptz_pose, struct fit_calib_detect_pose &detect_pose, int index);	
	
	void set_sample_size(int value);
	int get_sample_size();
	void clear_samples();
	
	int compute();
	void to_ptz_pose(struct stereo_detect_box &detect_box, struct fit_calib_ptz_pose &ptz_pose);
	
	int set_paras(std::string &value);
	int get_paras(std::string &value);
	
	
protected:	 
	void to_detect_pose(struct stereo_detect_box &detect_box, struct fit_calib_detect_pose &detect_pose);

protected:
	int fit_mode_;
	int coord_;
	int degree_[FIT_CALIB_PTZ_MAX_CHANNEL];
	 
	std::vector<std::pair<float, float> > samples_[FIT_CALIB_PTZ_MAX_CHANNEL];
	poly_fit *fits_[FIT_CALIB_PTZ_MAX_CHANNEL];
	
	

};





















#endif

