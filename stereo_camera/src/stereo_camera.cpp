#include "stereo_camera.h"

#include "discovery_receiver.h"
#include "command_client.h"
#include "stream_receiver.h"



stereo_camera::stereo_camera(const char *device_name, int port, int poll_time)
{
	open_ = 0;
	xfind = new discovery_receiver(device_name, port, poll_time);
	xcmd_ = new command_client;
	xstream_ = new stream_receiver;
}


stereo_camera::~stereo_camera()
{
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
	xcmd_->set_connect(ip, cmd_port);
	ret = xstream_->connect_stream(ip, stream_port, stream_index);
	if (ret < 0)
		return -1;
	open_ = 1;
	return 0;
}	
	
int stereo_camera::close_device()
{
	open_ = 0;
	return xstream_->disconnect_stream();
}

int stereo_camera::is_opened()
{
	return open_;
}

int stereo_camera::set_value(const char *cmd, int value, int timeout)
{
	return xcmd_->set_value(cmd, value, timeout);
}

int stereo_camera::set_value(const char *cmd, float value, int timeout)
{
	return xcmd_->set_value(cmd, value, timeout);
}

int stereo_camera::set_value(const char *cmd, std::string value, int timeout)
{
	return xcmd_->set_value(cmd, value, timeout);
}

int stereo_camera::set_poly_mask(std::vector<std::pair<float, float> > &value, int timeout)
{
	return xcmd_->set_poly_mask(value, timeout);
}


int stereo_camera::get_value(const char *cmd, int &value, int timeout)
{
	return xcmd_->get_value(cmd, value, timeout);
}

int stereo_camera::get_value(const char *cmd, float &value, int timeout)
{
	return xcmd_->get_value(cmd, value, timeout);
}

int stereo_camera::get_value(const char *cmd, std::string &value, int timeout)
{
	return xcmd_->get_value(cmd, value, timeout);
}

int stereo_camera::get_poly_mask(std::vector<std::pair<float, float> > &value, int timeout)
{
	return xcmd_->get_poly_mask(value, timeout);
}

int stereo_camera::do_action(const char *para, int timeout)
{
	return xcmd_->do_action(para, timeout);
}
 
int stereo_camera::get_pixel_point(int x, int y, struct stereo_pixel_point &value, int timeout)
{
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





















