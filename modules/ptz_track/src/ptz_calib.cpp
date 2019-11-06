#include "ptz_calib.h"



using namespace std;

ptz_calib::ptz_calib(int debug)
{
	debug_ = 0;
	coord_ = FIT_CALIB_BALL_COORD;
	
	fits_[PTZ_PAN_CHANNEL] = new poly_fit(debug);
	fits_[PTZ_TILT_CHANNEL] = new poly_fit(debug);
	fits_[PTZ_ZOOM_CHANNEL] = new poly_fit(debug);
	
	degree_[PTZ_PAN_CHANNEL] = 1;
	degree_[PTZ_TILT_CHANNEL] = 1;
	degree_[PTZ_ZOOM_CHANNEL] = 1;
}


ptz_calib::~ptz_calib()
{
	delete fits_[PTZ_PAN_CHANNEL];
	delete fits_[PTZ_TILT_CHANNEL];
	delete fits_[PTZ_ZOOM_CHANNEL];
}


int ptz_calib::compute(std::vector<std::pair<struct stereo_ptz_pose, struct stereo_detect_box> > &samples)
{
	if (samples.size() < 3) 
	{
		printf("ptz_calib: samples is %d pairs, too little, need more!\n", samples.size());
		return -1;
	}
	
	vector<pair<float, float> > samps[PTZ_MAX_CHANNEL];
	for (int i = 0; i < samples.size(); i++)
	{
		struct stereo_ptz_pose &ptz_pose = samples[i].first;
		struct stereo_detect_box &detect_box = samples[i].second;
		struct stereo_ptz_pose detect_pose = {0};
		sample_detect_pose(detect_box, detect_pose);
		for (int j = 0; j < PTZ_MAX_CHANNEL; j++)
		{
			samps[j].push_back(make_pair(detect_pose.val[j], ptz_pose.val[j]));
		}	
	}	

	for (int i = 0; i < PTZ_MAX_CHANNEL; i++)
	{
		ret = fits_[i]->compute(samps[i], degree_[i]);
		if (ret < 0) {
			printf("fit compute error!\n");
			return -1;
		}
			
	}	
	
	return 0;
}	




void ptz_calib::sample_detect_pose(struct stereo_detect_box &detect_box, struct stereo_ptz_pose &detect_pose)
{
	switch(coord_)
	{
		case PTZ_CALIB_GRAPH_COORD:
		{
			detect_pose.val[PTZ_PAN_CHANNEL] = (float)detect_box.box_x;
			detect_pose.val[PTZ_TILT_CHANNEL] = (float)detect_box.box_y;
			detect_pose.val[PTZ_ZOOM_CHANNEL] = (float)detect_box.box_w;
		}break; 
		case PTZ_CALIB_DEPTH_COORD:
		{
			detect_pose.val[PTZ_PAN_CHANNEL] = (float)detect_box.x;
			detect_pose.val[PTZ_TILT_CHANNEL] = (float)detect_box.y;
			detect_pose.val[PTZ_ZOOM_CHANNEL] = (float)detect_box.d;
		}break;
		case PTZ_CALIB_CAMEAR_COORD:
		{
			detect_pose.val[PTZ_PAN_CHANNEL] = (float)detect_box.xcm;
			detect_pose.val[PTZ_TILT_CHANNEL] = (float)detect_box.ycm;
			detect_pose.val[PTZ_ZOOM_CHANNEL] = (float)detect_box.zcm;
		}break;
		case PTZ_CALIB_ROOM_COORD:
		{
			detect_pose.val[PTZ_PAN_CHANNEL] = (float)detect_box.xtcm;
			detect_pose.val[PTZ_TILT_CHANNEL] = (float)detect_box.ytcm;
			detect_pose.val[PTZ_ZOOM_CHANNEL] = (float)detect_box.ztcm;
		}break;
		case PTZ_CALIB_BALL_COORD:
		{
			detect_pose.val[PTZ_PAN_CHANNEL] = (float)detect_box.xa;
			detect_pose.val[PTZ_TILT_CHANNEL] = (float)detect_box.ya;
			detect_pose.val[PTZ_ZOOM_CHANNEL] = (float)detect_box.r;
		}break;
		default:
			break;
	}
}

void ptz_calib::calc_ptz_pose(struct stereo_detect_box &detect_box, struct stereo_ptz_pose &ptz_pose)
{
	struct fit_calib_detect_pose detect_pose = {0};
	sample_detect_pose(detect_box, detect_pose);
	for (int i = 0; i < FIT_CALIB_PTZ_MAX_CHANNEL; i++)
	{
		ptz_pose.val[i] = fits_[i]->calc_fit(detect_pose.val[i]);
	}	
	
	if (debug_)
	{
		printf("(%d %d %d) -> (%d %d %d)\n", detect_pose.val[0], detect_pose.val[1], detect_pose.val[2], 
											ptz_pose.val[0], ptz_pose.val[1], ptz_pose.val[2]);
	}	
}

