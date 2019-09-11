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
	int set_sample(struct fit_calib_pose_sample &pose_sample, struct fit_calib_pixel_sample &pixel_sample, int index = -1);
	int get_sample(int pan_pose, int tilt_pose, int zoom_pose, struct stereo_pixel_point &pixel, int index);
	int get_sample(struct fit_calib_pose_sample &pose_sample, struct fit_calib_pixel_sample &pixel_sample, int index);
	void set_samples(struct fit_calib_samples &value) {samples_ = value;}
	struct fit_calib_samples & get_samples() {return samples_;}
	void clear_samples();
	int get_sample_count();
	void to_pose_sample(int pan_pose, int tilt_pose, int zoom_pose, struct fit_calib_pose_sample &pose_sample);
	void to_pixel_sample(struct stereo_pixel_point &pixel, struct fit_calib_pixel_sample &pixel_sample);
	void from_pose_sample(struct fit_calib_pose_sample &pose_sample, int &pan_pose, int &tilt_pose, int &zoom_pose);
	void from_pixel_sample(struct fit_calib_pixel_sample &pixel_sample, struct stereo_pixel_point &pixel);
	
	int set_degree(int coord, int channel, int value);
	int get_degree(int coord, int channel, int &value);
 
protected:
	int calc_fit_para(double *first, double *second, int count, std::vector<double> &para);
	
	
protected:	
	 
	struct fit_calib_samples samples_;
	
	std::vector<double> paras_[FIT_CALIB_MAX_COORD][FIT_CALIB_PTZ_MAX_CHANNEL][2];
};















#endif

