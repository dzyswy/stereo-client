#ifndef __JSON_GYRO_ANGLE_H
#define __JSON_GYRO_ANGLE_H


#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <exception>
#include <string>

#include "stream_struct.h"

struct json_gyro_angle
{
	struct stereo_gyro_angle gyro_angle;
	
	json_gyro_angle(struct stereo_gyro_angle &value);
	
	int to_string(std::string &value);
	int from_string(std::string &value);
};











#endif



















