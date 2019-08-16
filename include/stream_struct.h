#ifndef __STREAM_STRUCT_H
#define __STREAM_STRUCT_H






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

struct stereo_gyro_angle
{
	float roll;
	float pitch;
};
 







#endif









