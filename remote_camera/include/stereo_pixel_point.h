#ifndef __STEREO_PIXEL_POINT_H
#define __STEREO_PIXEL_POINT_H


#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <exception>
#include <string>






struct stereo_pixel_point
{
	int x;
	int y;
	int d;
	int xcm;
	int ycm;
	int zcm;
	int xtcm;
	int ytcm;
	int ztcm;
	double xa;
	double ya;
	double r;
	
	int to_string(std::string &value);
	int from_string(std::string &value);
	
	
};











#endif



















