#include "media_record.h"

#include "avilib.h"
#include "piclib.h"

using namespace std;
using namespace std::chrono;

media_record::media_record()
{
	avifile = NULL;
}

media_record::~media_record()
{
	close_media();
}


int media_record::open_media(const char *file_name, int width, int height, double fps)
{
	std::unique_lock<std::mutex> lock(mux_);
	
	if (avifile) {
		cout << "record is busy, please close first!\n";
		return -1;
	}
		
	avifile = AVI_open_output_file(file_name);
	if (!avifile)
		return -1;
	
	AVI_set_video((avi_t *)avifile, width, height, fps, "MJPG");
	avi_width_ = width;
	avi_height_ = height;
	avi_fps_ = fps;
	
	frame_count_ = 0;
	t_[0] = steady_clock::now();
	
	return 0;
}


int media_record::close_media()
{
	std::unique_lock<std::mutex> lock(mux_);
	
	if (!avifile)
		return -1;
	
	t_[1] = steady_clock::now();
	double sec = duration_cast<duration<double>>(t_[1] - t_[0]).count();
	avi_fps_ = (double)frame_count_ / sec;
	AVI_set_video((avi_t *)avifile, avi_width_, avi_height_, avi_fps_, "MJPG");
	cout << "record avi fps: " << avi_fps_ << endl;
	AVI_close((avi_t *)avifile);
	avifile = NULL;
	return 0;
}


int media_record::write_frame(char *data, long bytes, int keyframe)
{
	int ret;
	ret = AVI_write_frame((avi_t *)avifile, data, bytes, keyframe);
	if (ret < 0)
	{
		close_media();
		return -1;
	}	
	frame_count_++;
	return 0;
}


int media_record::save_picture(const char *file_name, unsigned char *buf, int size)
{
	return get_picture(file_name, buf, size);
}











