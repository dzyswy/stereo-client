#ifndef __STEREO_GYRO_ANGLE_H
#define __STEREO_GYRO_ANGLE_H


#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <exception>
#include <string>






struct stereo_gyro_angle
{
	float xabs;
	float yabs;
	float zabs;
	float roll;
	float pitch;
	
	int to_string(std::string &value);
	int from_string(std::string &value);
	
	
};











#endif



















