#include "stereo_camera.h"
#include "rpc_client.h"
#include "stream_client.h"
#include "json_detect_boxes.h"
#include "json_gyro_angle.h"
#include "json_float_pairs.h"
#include "json_ptz_samples.h"

using namespace std;





stereo_camera::stereo_camera(int debug)
{
	open_ = 0;
	xcmd_ = new rpc_client(debug);
	xstream_ = new stream_client(debug);
}


stereo_camera::~stereo_camera()
{
	close_device();
	delete xstream_;
	delete xcmd_;
}

int stereo_camera::open_device(const char *ip, int cmd_port)
{
	int ret;
	std::unique_lock<std::mutex> lock(mux_);
	
	if (open_)
		return -1;
 
	ret = xcmd_->set_connect(ip, cmd_port); 
	if (ret < 0)
		return -1;
	open_ = 1;

	return 0;
}	


int stereo_camera::open_device(const char *ip, int cmd_port, int stream_port, int stream_index)
{
	int ret;
	std::unique_lock<std::mutex> lock(mux_);
	
	if (open_)
		return -1;
 
	ret = xcmd_->set_connect(ip, cmd_port);
	if (ret < 0)
		return -1;
	ret = xstream_->connect_stream(ip, stream_port, stream_index);
	if (ret < 0)
		return -1;
	
	open_ = 1;

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


int stereo_camera::set_value(const char *key, int value, int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (!open_)
		return -1;
	return xcmd_->set_value(key, value, timeout);
}

int stereo_camera::set_value(const char *key, float value, int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (!open_)
		return -1;
	
	return xcmd_->set_value(key, value, timeout);
}

int stereo_camera::set_value(const char *key, std::string value, int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (!open_)
		return -1;
	
	return xcmd_->set_value(key, value, timeout);
}

int stereo_camera::set_poly_mask(std::vector<std::pair<float, float> > &value, int timeout)
{
	int ret;
	std::unique_lock<std::mutex> lock(mux_);
	if (!open_)
		return -1;
	
	json_float_pairs temp(value);
	string val = "";
	ret = temp.to_string(val);
	if (ret < 0)
		return -1;
	
	return xcmd_->set_value("poly_mask", val, timeout);
}

int stereo_camera::set_ptz_samples(std::vector<std::pair<struct stereo_ptz_pose, struct stereo_detect_box> > &samples, int timeout)
{
	int ret;
	std::unique_lock<std::mutex> lock(mux_);
	if (!open_)
		return -1;
	
	printf("samples:\n");
	for (int i = 0; i < samples.size(); i++)
	{
		printf("%d: (%d %d %d) -> (%d %d %d)\n", i, samples[i].first.val[0], samples[i].first.val[1], samples[i].first.val[2], 
			samples[i].second.x, samples[i].second.y, (int)samples[i].second.r);
	}	
	
	json_ptz_samples temp(samples);
	string val = "";
	ret = temp.to_string(val);
	if (ret < 0)
		return -1;
	
	return xcmd_->set_value("ptz_samples", val, timeout);
}

int stereo_camera::get_value(const char *key, int &value, int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (!open_)
		return -1;
	
	return xcmd_->get_value(key, value, timeout);
}

int stereo_camera::get_value(const char *key, float &value, int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (!open_)
		return -1;
	
	return xcmd_->get_value(key, value, timeout);
}

int stereo_camera::get_value(const char *key, std::string &value, int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (!open_)
		return -1;
	
	return xcmd_->get_value(key, value, timeout);
}

int stereo_camera::get_value(const char *key, const std::string &para, std::string &value, int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (!open_)
		return -1;
	
	return xcmd_->get_value(key, para, value, timeout);
}

int stereo_camera::get_poly_mask(std::vector<std::pair<float, float> > &value, int timeout)
{
	int ret;
	std::unique_lock<std::mutex> lock(mux_);
	if (!open_)
		return -1;
	
	string val = "";
	ret = xcmd_->get_value("poly_mask", val, timeout);
	if (ret < 0)
		return -1;
	
	json_float_pairs temp;
	ret = temp.from_string(val);
	if (ret < 0)
		return -1;
	
	value = temp.to_struct();
	
	return 0;
}

int stereo_camera::do_action(const char *para, int timeout)
{
	std::unique_lock<std::mutex> lock(mux_);
	if (!open_)
		return -1;
	
	return xcmd_->do_action(para, timeout);
}
 
int stereo_camera::get_detect_point(int x, int y, struct stereo_detect_box &value, int timeout)
{
	int ret;
	
	std::unique_lock<std::mutex> lock(mux_);
	if (!open_)
		return -1;
	stringstream os;
	os << x << "," << y;
	string val = os.str();
	string result = "";
	ret = xcmd_->get_value("detect_point", val, result, timeout);
	if (ret < 0)
		return -1;
	
	json_detect_boxes temp;
	ret = temp.from_string(result);
	if (ret < 0)
		return -1;
	
	std::vector<struct stereo_detect_box> detect_boxes;
	detect_boxes = temp.to_struct();
	value = detect_boxes[0];
	
	return 0;
}


int stereo_camera::query_frame(int timeout)
{
	return xstream_->query_frame(timeout);
}

void stereo_camera::get_image(std::vector<unsigned char> &image)
{
	xstream_->get_image(image);
}

int stereo_camera::get_frame_count(int &frame_count)
{
	int ret;
	string result = "";
	ret = xstream_->get_header("frame_count", result);
	if (ret < 0)
		return -1;
	
	frame_count = atoi(result.c_str());
	return 0;
}

int stereo_camera::get_detect_boxes(std::vector<struct stereo_detect_box> &detect_boxes)
{
	int ret;
	string result = "";
	ret = xstream_->get_header("detect_boxes", result);
	if (ret < 0)
		return -1;
	
	
	json_detect_boxes temp;
	ret = temp.from_string(result);
	if (ret < 0)
		return -1;
	
	detect_boxes = temp.to_struct();
	return 0;
}

int stereo_camera::get_gyro_angle(struct stereo_gyro_angle &gyro_angle)
{
	int ret;
	string result = "";
	ret = xstream_->get_header("gyro_angle", result);
	if (ret < 0)
		return -1;
	
	json_gyro_angle temp;
	ret = temp.from_string(result);
	if (ret < 0)
		return -1;
	
	gyro_angle = temp.to_struct();
	return 0;
}

int stereo_camera::get_reconnect_count()
{
	return xstream_->get_reconnect_count();
}










