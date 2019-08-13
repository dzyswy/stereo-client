#include "stereo_stream.h"






stereo_stream::stereo_stream()
{
	going = 0;
}

stereo_stream::~stereo_stream()
{
	disconnect_stream();
}

int stereo_stream::connect_stream(const char *ip, int port, int stream_mode)
{
	std::unique_lock<std::mutex> lock(mux_);
	
	if (going)
		return -1;
	
	ip_ = ip;
	port_ = port;
	
	going = 1;
	
	run_thread_ = new std::thread([this] () {stream_process();});

	return 0;
}

int stereo_stream::disconnect_stream()
{
	std::unique_lock<std::mutex> lock(mux_);
	
	going = 0;
	if (run_thread_)
	{
		run_thread_->join();
		delete run_thread_;
		run_thread_ = NULL;
	}
	
	return 0;
}

int stereo_stream::query_frame(int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (cond_.wait_for(lock, std::chrono::seconds(timeout)) == std::cv_status::timeout)
		return -1;
	
	return 0;
}

void stereo_stream::get_frame(std::vector<unsigned char> &image)
{
	std::unique_lock<std::mutex> lock(mux_);
	image = frame_buffer_;
}

void stereo_stream::get_detect_boxes(vector<struct http_output_detect_box> &detect_boxes)
{
	std::unique_lock<std::mutex> lock(mux_);
	detect_boxes = detect_boxes_;
}

void stereo_stream::get_gyro_angle(struct gyro_status &gyro_angle)
{
	std::unique_lock<std::mutex> lock(mux_);
	gyro_angle = gyro_angle_;
}

void stereo_stream::stream_process()
{
	int ret;
	stream_receiver *stream = new stream_receiver(ip_, port_, stream_port_);
	while(1)
	{
		{
			std::unique_lock<std::mutex> lock(mux_);
			if (!going)
				break;
		}
		
		ret = stream->query_frame(5);
		if (ret < 0)
		{
			delete stream;
			stream = new stream_receiver(ip_, port_, stream_port_);
			continue;
		}	
		
		{
			std::unique_lock<std::mutex> lock(mux_);		
			stream->get_frame(frame_buffer_);
			stream->get_detect_boxes(detect_boxes_);
			stream->get_gyro_angle(gyro_angle_);
			cond_.notify_all();		
		}
		
	}
	delete stream;
}










