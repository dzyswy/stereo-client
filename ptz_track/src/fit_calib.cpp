#include "fit_calib.h"
#include "linearfit.h"
#include "polynomialfit.h"
#include "ptz_ctl_visca.h"
#include "stereo_camera.h"






using namespace std;






fit_calib::fit_calib()
{
 
	set_degree(FIT_CALIB_GRAPH_COORD, FIT_CALIB_PTZ_PAN, 3);
	set_degree(FIT_CALIB_GRAPH_COORD, FIT_CALIB_PTZ_TILT, 3);
	set_degree(FIT_CALIB_GRAPH_COORD, FIT_CALIB_PTZ_ZOOM, 3);
	
	set_degree(FIT_CALIB_CAMEAR_COORD, FIT_CALIB_PTZ_PAN, 3);
	set_degree(FIT_CALIB_CAMEAR_COORD, FIT_CALIB_PTZ_TILT, 3);
	set_degree(FIT_CALIB_CAMEAR_COORD, FIT_CALIB_PTZ_ZOOM, 3);
	
	set_degree(FIT_CALIB_ROOM_COORD, FIT_CALIB_PTZ_PAN, 3);
	set_degree(FIT_CALIB_ROOM_COORD, FIT_CALIB_PTZ_TILT, 3);
	set_degree(FIT_CALIB_ROOM_COORD, FIT_CALIB_PTZ_ZOOM, 3);
	
	set_degree(FIT_CALIB_BALL_COORD, FIT_CALIB_PTZ_PAN, 3);
	set_degree(FIT_CALIB_BALL_COORD, FIT_CALIB_PTZ_TILT, 3);
	set_degree(FIT_CALIB_BALL_COORD, FIT_CALIB_PTZ_ZOOM, 3);
	
	
}
 
int fit_calib::pose_to_pixel(float pose, float &pixel, int coord, int channel)
{
	int ret;
	int degree = 0;
	ret = get_degree(coord, channel, degree);
	if (ret < 0)
		return -1;
	
	double sum = paras_[coord][channel][0][degree];
	for (int i = 0; i < degree; i++)
	{
		sum += pow(pose, degree - i) * paras_[coord][channel][0][i];
	}	
	pixel = (float)sum;
	return 0;
}

int fit_calib::pixel_to_pose(float pixel, float &pose, int coord, int channel)
{
	int ret;
	int degree = 0;
	ret = get_degree(coord, channel, degree);
	if (ret < 0)
		return -1;
	
	double sum = paras_[coord][channel][1][degree];
	for (int i = 0; i < degree; i++)
	{
		sum += pow(pixel, degree - i) * paras_[coord][channel][1][i];
	}	
	pose = (float)sum;
	return 0;
}

int fit_calib::pixel_to_pose(struct fit_calib_stereo_pixel &pixel, struct fit_calib_ptz_pose &pose, int coord)
{
	for (int i = 0; i < FIT_CALIB_PTZ_MAX_CHANNEL; i++)
	{
		int ret = pixel_to_pose(pixel.val[coord][i], pose.val[i], coord, i);
		if (ret < 0)
			return -1;
	}
	
	return 0;
}

int fit_calib::gen_para()
{
	int ret = 0;
	int sample_count = get_sample_count();
	double *first = new double [sample_count];
	double *second = new double [sample_count];
	
	for (int i = 0; i < FIT_CALIB_MAX_COORD; i++)
	{
		for (int j = 0; j < FIT_CALIB_PTZ_MAX_CHANNEL; j++)
		{
			for (int k = 0; k < sample_count; k++)
			{
				std::pair<struct fit_calib_ptz_pose, struct fit_calib_stereo_pixel> &sample = samples_.samples[k];
				
				first[k] = (double)sample.first.val[j];
				second[k] = (double)sample.second.val[i][j];
			}	
			ret = calc_fit_para(first, second, sample_count, paras_[i][j][0]);
			if (ret < 0)
				break;
			ret = calc_fit_para(second, first, sample_count, paras_[i][j][1]);
			if (ret < 0)
				break;
		}
		if (ret < 0)
			break;		
	}

	delete first;
	delete second;
	if (ret < 0)
		return -1;
	return 0;
}


int fit_calib::calc_fit_para(double *first, double *second, int count, vector<double> &para)
{
	int degree = para.size() - 1;
	Polynomial pf;
	pf.setAttribute(degree, false, 1.0);
	pf.setSample(first, second, count, false, NULL);
	if (!pf.process()) {
		return -1;
	}
	pf.print();
	for (int i = 0; i < para.size(); i++)
	{
		para[i] = pf.getResult(i);
	}
	return 0;
}

 
int fit_calib::set_degree(int coord, int channel, int value) 
{
	if ((coord < 0) || (coord >= FIT_CALIB_MAX_COORD))
		return -1;
	if ((channel < 0) || (channel >= FIT_CALIB_PTZ_MAX_CHANNEL))
		return -1;
	 
	paras_[coord][channel][0].resize(value);
	paras_[coord][channel][1].resize(value);
	return 0;
}

int fit_calib::get_degree(int coord, int channel, int &value)
{
	if ((coord < 0) || (coord >= FIT_CALIB_MAX_COORD))
		return -1;
	if ((channel < 0) || (channel >= FIT_CALIB_PTZ_MAX_CHANNEL))
		return -1;
	
	value = paras_[coord][channel][0].size();
	return 0;
}

int fit_calib::set_sample(int pan_pose, int tilt_pose, int zoom_pose, struct stereo_pixel_point &pixel, int index)
{
	int sample_count = get_sample_count();
	if (index >= sample_count)
		return -1;
	
	struct fit_calib_ptz_pose pose_sample;
	to_pose_sample(pan_pose, tilt_pose, zoom_pose, pose_sample);
	
	
	struct fit_calib_stereo_pixel pixel_sample;
	to_pixel_sample(pixel, pixel_sample); 
	
	if (index < 0) {
		samples_.samples.push_back(make_pair(pose_sample, pixel_sample));
	} else {
		samples_.samples[index] = make_pair(pose_sample, pixel_sample);
	}
	return 0;
}

int fit_calib::set_sample(struct fit_calib_ptz_pose &pose_sample, struct fit_calib_stereo_pixel &pixel_sample, int index)
{
	int sample_count = get_sample_count();
	if (index >= sample_count)
		return -1;
	
	if (index < 0) {
		samples_.samples.push_back(make_pair(pose_sample, pixel_sample));
	} else {
		samples_.samples[index] = make_pair(pose_sample, pixel_sample);
	}
	return 0;
}

int fit_calib::get_sample(int pan_pose, int tilt_pose, int zoom_pose, struct stereo_pixel_point &pixel, int index)
{
	int sample_count = get_sample_count();
	if ((index < 0) || (index >= sample_count))
		return -1;
	
	struct fit_calib_ptz_pose &pose_sample = samples_.samples[index].first;
	from_pose_sample(pose_sample, pan_pose, tilt_pose, zoom_pose);
	
	
	struct fit_calib_stereo_pixel &pixel_sample = samples_.samples[index].second;
	from_pixel_sample(pixel_sample, pixel);
	
	return 0;
}

int fit_calib::get_sample(struct fit_calib_ptz_pose &pose_sample, struct fit_calib_stereo_pixel &pixel_sample, int index)
{
	int sample_count = get_sample_count();
	if ((index < 0) || (index >= sample_count))
		return -1;
	
	
	pose_sample = samples_.samples[index].first;
	pixel_sample = samples_.samples[index].second;
	return 0;
}

void fit_calib::clear_samples()
{
	samples_.samples.clear();
}
 
int fit_calib::get_sample_count()
{
	return samples_.samples.size();
}


void fit_calib::to_pose_sample(int pan_pose, int tilt_pose, int zoom_pose, struct fit_calib_ptz_pose &pose_sample)
{
	pose_sample.val[FIT_CALIB_PTZ_PAN] = (float)pan_pose;
	pose_sample.val[FIT_CALIB_PTZ_TILT] = (float)tilt_pose;
	pose_sample.val[FIT_CALIB_PTZ_ZOOM] = (float)zoom_pose;
}

void fit_calib::to_pixel_sample(struct stereo_pixel_point &pixel, struct fit_calib_stereo_pixel &pixel_sample)
{
	pixel_sample.val[FIT_CALIB_GRAPH_COORD][FIT_CALIB_PTZ_PAN] = pixel.x;
	pixel_sample.val[FIT_CALIB_GRAPH_COORD][FIT_CALIB_PTZ_TILT] = pixel.y;
	pixel_sample.val[FIT_CALIB_GRAPH_COORD][FIT_CALIB_PTZ_ZOOM] = pixel.d;
	
	pixel_sample.val[FIT_CALIB_CAMEAR_COORD][FIT_CALIB_PTZ_PAN] = pixel.xcm;
	pixel_sample.val[FIT_CALIB_CAMEAR_COORD][FIT_CALIB_PTZ_TILT] = pixel.ycm;
	pixel_sample.val[FIT_CALIB_CAMEAR_COORD][FIT_CALIB_PTZ_ZOOM] = pixel.zcm;
	
	pixel_sample.val[FIT_CALIB_ROOM_COORD][FIT_CALIB_PTZ_PAN] = pixel.xtcm;
	pixel_sample.val[FIT_CALIB_ROOM_COORD][FIT_CALIB_PTZ_TILT] = pixel.ytcm;
	pixel_sample.val[FIT_CALIB_ROOM_COORD][FIT_CALIB_PTZ_ZOOM] = pixel.ztcm;
	
	pixel_sample.val[FIT_CALIB_BALL_COORD][FIT_CALIB_PTZ_PAN] = pixel.xa;
	pixel_sample.val[FIT_CALIB_BALL_COORD][FIT_CALIB_PTZ_TILT] = pixel.ya;
	pixel_sample.val[FIT_CALIB_BALL_COORD][FIT_CALIB_PTZ_ZOOM] = pixel.r;
}

void fit_calib::from_pose_sample(struct fit_calib_ptz_pose &pose_sample, int &pan_pose, int &tilt_pose, int &zoom_pose)
{
	pan_pose = (int)pose_sample.val[FIT_CALIB_PTZ_PAN];
	tilt_pose = (int)pose_sample.val[FIT_CALIB_PTZ_TILT];
	zoom_pose = (int)pose_sample.val[FIT_CALIB_PTZ_ZOOM];
}

void fit_calib::from_pixel_sample(struct fit_calib_stereo_pixel &pixel_sample, struct stereo_pixel_point &pixel)
{
	pixel.x = pixel_sample.val[FIT_CALIB_GRAPH_COORD][FIT_CALIB_PTZ_PAN];
	pixel.y = pixel_sample.val[FIT_CALIB_GRAPH_COORD][FIT_CALIB_PTZ_TILT];
	pixel.d = pixel_sample.val[FIT_CALIB_GRAPH_COORD][FIT_CALIB_PTZ_ZOOM];
	
	pixel.xcm = pixel_sample.val[FIT_CALIB_CAMEAR_COORD][FIT_CALIB_PTZ_PAN];
	pixel.ycm = pixel_sample.val[FIT_CALIB_CAMEAR_COORD][FIT_CALIB_PTZ_TILT];
	pixel.zcm = pixel_sample.val[FIT_CALIB_CAMEAR_COORD][FIT_CALIB_PTZ_ZOOM];
	
	pixel.xtcm = pixel_sample.val[FIT_CALIB_ROOM_COORD][FIT_CALIB_PTZ_PAN];
	pixel.ytcm = pixel_sample.val[FIT_CALIB_ROOM_COORD][FIT_CALIB_PTZ_TILT];
	pixel.ztcm = pixel_sample.val[FIT_CALIB_ROOM_COORD][FIT_CALIB_PTZ_ZOOM];
	
	pixel.xa = pixel_sample.val[FIT_CALIB_BALL_COORD][FIT_CALIB_PTZ_PAN];
	pixel.ya = pixel_sample.val[FIT_CALIB_BALL_COORD][FIT_CALIB_PTZ_TILT];
	pixel.r = pixel_sample.val[FIT_CALIB_BALL_COORD][FIT_CALIB_PTZ_ZOOM];
}







