#ifndef __STEREO_DETECT_BOXES_H
#define __STEREO_DETECT_BOXES_H


#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <exception>
#include <string>
#include <vector>


struct stereo_detect_box
{
	int id;
	int box_x;
	int box_y;
	int box_w;
	int box_h;
	int x;
	int y;
	int d;
	int xcm;
	int ycm;
	int zcm;
	int xtcm;
	int ytcm;
	int ztcm;
	float xa;
	float ya;
	float r;
	
};


struct stereo_detect_boxes
{
	std::vector<struct stereo_detect_box> detect_boxes;
	
	int to_string(std::string &value);
	int from_string(std::string &value);
};











#endif



















