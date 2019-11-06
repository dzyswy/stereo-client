#ifndef __JSON_PTZ_SAMPLES_H
#define __JSON_PTZ_SAMPLES_H


#include "stereo_struct.h"




class json_ptz_samples
{
public:
	json_ptz_samples();
	json_ptz_samples(std::vector<std::pair<struct stereo_ptz_pose, struct stereo_detect_box> > &value);
	std::vector<std::pair<struct stereo_ptz_pose, struct stereo_detect_box> > to_struct();
	
	int from_string(std::string &value);
	int to_string(std::string &value);

protected: 
	std::vector<std::pair<struct stereo_ptz_pose, struct stereo_detect_box> > ptz_samples_;
};
















#endif



