#ifndef __JSON_PIXEL_POINT_H
#define __JSON_PIXEL_POINT_H


#include "stereo_struct.h"




class json_pixel_point
{
public:
	json_pixel_point();
	json_pixel_point(struct stereo_pixel_point &value);
	struct stereo_pixel_point to_struct();
	
	int from_string(std::string &value);
	int to_string(std::string &value);

protected:
	struct stereo_pixel_point data_;
};
















#endif



