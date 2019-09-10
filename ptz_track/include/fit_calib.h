#ifndef __FIT_CALIB_H
#define __FIT_CALIB_H




#include "fit_calib_samples.h"




class fit_calib
{
public:
	fit_calib();
 
	int pose_to_pixel(float pose, float &pixel, int coord, int channel);
	int pixel_to_pose(float pixel, float &pose, int coord, int channel);
 
	int gen_para();
	int set_sample(int pan_pose, int tilt_pose, int zoom_pose, struct stereo_pixel_point &pixel, int index = -1);
	int get_sample(int pan_pose, int tilt_pose, int zoom_pose, struct stereo_pixel_point &pixel, int index);
	void clear_samples();
	int get_sample_count();
	
	int set_degree(int coord, int channel, int value);
	int get_degree(int coord, int channel, int &value);
 
protected:
	int calc_fit_para(float *first, float *second, int count, vector<double> &para);
	
	
protected:	
	 
	struct fit_calib_samples samples_;
	
	vector<double> paras_[FIT_CALIB_MAX_COORD][FIT_CALIB_PTZ_MAX_CHANNEL][2];
};















#endif

