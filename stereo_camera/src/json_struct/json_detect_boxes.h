#ifndef __JSON_DETECT_BOXES_H
#define __JSON_DETECT_BOXES_H


#include "stereo_struct.h"




class json_detect_boxes
{
public:
	json_detect_boxes();
	json_detect_boxes(std::vector<struct stereo_detect_box> &value);
	std::vector<struct stereo_detect_box> to_struct();
	
	int from_string(std::string &value);
	int to_string(std::string &value);

protected:
	std::vector<struct stereo_detect_box> detect_boxes_;
};
















#endif



