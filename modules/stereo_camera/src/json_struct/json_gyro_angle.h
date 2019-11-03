#ifndef __JSON_GYRO_ANGLE_H
#define __JSON_GYRO_ANGLE_H


#include "stereo_struct.h"




class json_gyro_angle
{
public:
	json_gyro_angle();
	json_gyro_angle(struct stereo_gyro_angle &value);
	struct stereo_gyro_angle to_struct();
	
	int from_string(std::string &value);
	int to_string(std::string &value);

protected:
	struct stereo_gyro_angle data_;
};
















#endif



