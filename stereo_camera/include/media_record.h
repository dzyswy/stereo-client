#ifndef __MEDIA_RECORD_H
#define __MEDIA_RECORD_H

#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <ctime>
#include <ratio>
#include <chrono>


class media_record
{
public:
	media_record();
	~media_record();
	int open_media(const char *file_name, int width, int height, double fps);
	int close_media();
	int write_frame(char *data, long bytes, int keyframe);
	
	
	int save_picture(const char *file_name, unsigned char *buf, int size);
	
private:
	void *avifile;
	int avi_width_;
	int avi_height_;
	double avi_fps_;
	long frame_count_;
	std::chrono::steady_clock::time_point t_[2];
	std::mutex mux_;
};














#endif



