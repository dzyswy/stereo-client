#include "stereo_camera.h"

#include "discovery_receiver.h"
#include "command_client.h"
#include "stream_receiver.h"

using namespace std;

stereo_camera::stereo_camera(const char *device_name, int port, int poll_time, int debug)
{
	open_ = 0;
	xfind = new discovery_receiver(device_name, port, poll_time);
	xcmd_ = new command_client;
	xstream_ = new stream_receiver(debug);
}


stereo_camera::~stereo_camera()
{
	close_device();
	delete xcmd_;
	delete xstream_;
	delete xfind;
}

void stereo_camera::get_device_nodes(std::vector<std::string> &device_nodes)
{
	xfind->get_device_nodes(device_nodes);
}


int stereo_camera::open_device(const char *ip, int cmd_port, int stream_port, int stream_index)
{
	int ret;
	std::unique_lock<std::mutex> lock(mux_);
	
	if (open_)
		return -1;
 
	xcmd_->set_connect(ip, cmd_port);
	ret = xstream_->connect_stream(ip, stream_port, stream_index);
	if (ret < 0)
		return -1;
	open_ = 0;

	return 0;
}	
	
int stereo_camera::close_device()
{
	std::unique_lock<std::mutex> lock(mux_);
	open_ = 0;
	
	return xstream_->disconnect_stream();
}

int stereo_camera::is_opened()
{
	std::unique_lock<std::mutex> lock(mux_);
	return open_;
}


int stereo_camera::set_value(const char *cmd, int value, int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (!open_)
		return -1;
	return xcmd_->set_value(cmd, value, timeout);
}

int stereo_camera::set_value(const char *cmd, float value, int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (!open_)
		return -1;
	
	return xcmd_->set_value(cmd, value, timeout);
}

int stereo_camera::set_value(const char *cmd, std::string value, int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (!open_)
		return -1;
	
	return xcmd_->set_value(cmd, value, timeout);
}

int stereo_camera::set_poly_mask(std::vector<std::pair<float, float> > &value, int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (!open_)
		return -1;
	
	return xcmd_->set_poly_mask(value, timeout);
}


int stereo_camera::get_value(const char *cmd, int &value, int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (!open_)
		return -1;
	
	return xcmd_->get_value(cmd, value, timeout);
}

int stereo_camera::get_value(const char *cmd, float &value, int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (!open_)
		return -1;
	
	return xcmd_->get_value(cmd, value, timeout);
}

int stereo_camera::get_value(const char *cmd, std::string &value, int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (!open_)
		return -1;
	
	return xcmd_->get_value(cmd, value, timeout);
}

int stereo_camera::get_poly_mask(std::vector<std::pair<float, float> > &value, int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (!open_)
		return -1;
	
	return xcmd_->get_poly_mask(value, timeout);
}

int stereo_camera::do_action(const char *para, int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (!open_)
		return -1;
	
	return xcmd_->do_action(para, timeout);
}
 
int stereo_camera::get_pixel_point(int x, int y, struct stereo_pixel_point &value, int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (!open_)
		return -1;
	
	return xcmd_->get_pixel_point(x, y, value, timeout);
}


int stereo_camera::query_frame(int timeout)
{
	return xstream_->query_frame(timeout);
}

void stereo_camera::get_frame(std::vector<unsigned char> &image)
{
	xstream_->get_frame(image);
}

void stereo_camera::get_detect_boxes(std::vector<struct stereo_detect_box> &detect_boxes)
{
	xstream_->get_detect_boxes(detect_boxes);
}

void stereo_camera::get_gyro_angle(struct stereo_gyro_angle &gyro_angle)
{
	xstream_->get_gyro_angle(gyro_angle);
}

int stereo_camera::get_reconnect_count()
{
	return xstream_->get_reconnect_count();
}


void stereo_camera::detect_box_to_pixel(struct stereo_detect_box &detect_box, struct stereo_pixel_point &detect_pixel)
{
	memset(&detect_pixel, 0, sizeof(detect_pixel));
	detect_pixel.x = detect_box.x;
	detect_pixel.y = detect_box.y;
	detect_pixel.d = detect_box.d;
	detect_pixel.xcm = detect_box.xcm;
	detect_pixel.ycm = detect_box.ycm;
	detect_pixel.zcm = detect_box.zcm;
	detect_pixel.xtcm = detect_box.xtcm;
	detect_pixel.ytcm = detect_box.ytcm;
	detect_pixel.ztcm = detect_box.ztcm;
	detect_pixel.xa = detect_box.xa;
	detect_pixel.ya = detect_box.ya;
	detect_pixel.r = detect_box.r;
}

void stereo_camera::detect_pixel_to_box(struct stereo_pixel_point &detect_pixel, struct stereo_detect_box &detect_box)
{
	memset(&detect_box, 0, sizeof(detect_box));
	detect_box.x = detect_pixel.x;
	detect_box.y = detect_pixel.y;
	detect_box.d = detect_pixel.d;
	detect_box.xcm = detect_pixel.xcm;
	detect_box.ycm = detect_pixel.ycm;
	detect_box.zcm = detect_pixel.zcm;
	detect_box.xtcm = detect_pixel.xtcm;
	detect_box.ytcm = detect_pixel.ytcm;
	detect_box.ztcm = detect_pixel.ztcm;
	detect_box.xa = detect_pixel.xa;
	detect_box.ya = detect_pixel.ya;
	detect_box.r = detect_pixel.r;
}



















