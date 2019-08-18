#ifndef __JSON_DETECT_BOXES_H
#define __JSON_DETECT_BOXES_H


#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <exception>
#include <string>
#include <vector>


#include "stream_struct.h"


struct json_detect_boxes
{
	std::vector<struct stereo_detect_box> detect_boxes;
	
	void to_struct(std::vector<struct stereo_detect_box> &value);
	void from_struct(std::vector<struct stereo_detect_box> &value);
	
	int to_string(std::string &value);
	int from_string(std::string &value);
};











#endif



















