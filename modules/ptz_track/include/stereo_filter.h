#ifndef __STEREO_FILTER_H
#define __STEREO_FILTER_H


#include <iostream>
#include <cmath>


#include "stereo_camera.h"



enum stereo_filter_number_state_type
{
	STEREO_FILTER_NO_TARGET = 0,
	STEREO_FILTER_SINGLE_TARGET = 1,
	STEREO_FILTER_MULTI_TARGET = 2,
	STEREO_FILTER_TARGET_MAX_STATE = 3,
};


#define STEREO_FILTER_MAX_FOCUS_BOX_NUM			5


class stereo_filter
{
public:
	stereo_filter(stereo_camera *camera);
	
	void compute(std::vector<struct stereo_detect_box> &detect_boxes, int &number_state, struct stereo_detect_box &focus_box, int &stable_state);
	void clear_filter();

	

 
	
protected:
	stereo_camera *camera_;
	int frame_count_;
	
	int min_number_count_;
	int max_number_count_;  
	float stable_distance_;
	int min_stable_count_;
	
	int number_state_;
	int number_count_[STEREO_FILTER_TARGET_MAX_STATE];

	
	struct stereo_detect_box pre_focus_box_;
	struct stereo_detect_box focus_boxes_[STEREO_FILTER_MAX_FOCUS_BOX_NUM];
	int stable_count_;
	
	
public:
	void set_min_number_count(int value) 
	{
		min_number_count_ = value;
	}
	
	void set_max_number_count(int value)
	{
		max_number_count_ = value;
	}
	
	void set_stable_distance(float value)
	{
		stable_distance_ = value;
	}
	
	void set_min_stable_count(int value)
	{
		min_stable_count_ = value;
	}
	
	
	int get_min_number_count()
	{
		return min_number_count_;
	}
	
	int get_max_number_count()
	{
		return max_number_count_;
	}
	
	float get_stable_distance()
	{
		return stable_distance_;
	}
	
	int get_min_stable_count()
	{
		return min_stable_count_;
	}
	
};





#endif















