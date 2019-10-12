#include "fit_calib.h"
#include "poly_fit.h"
#include "linearfit.h"
#include "polynomialfit.h"
#include "ptz_ctl_visca.h"
#include "stereo_camera.h"
#include "json/json.h"





using namespace std;


fit_calib::fit_calib()
{
	for (int i = 0; i < FIT_CALIB_PTZ_MAX_CHANNEL; i++)
	{
		fits_[i] = new poly_fit;
	}	
	
	fit_mode_ = FIT_CALIB_CLOSE_TO_MODE;
	set_fit_mode(fit_mode_);
}

fit_calib::~fit_calib()
{
	for (int i = 0; i < FIT_CALIB_PTZ_MAX_CHANNEL; i++)
	{
		delete fits_[i];
	}
}

void fit_calib::set_fit_mode(int value)
{
	fit_mode_ = value;
	
	switch(fit_mode_)
	{
		case FIT_CALIB_CLOSE_TO_MODE:
		{
			coord_ = FIT_CALIB_BALL_COORD;
			degree_[FIT_CALIB_PTZ_PAN] = 1;
			degree_[FIT_CALIB_PTZ_TILT] = 1;
			degree_[FIT_CALIB_PTZ_ZOOM] = 3;
			
		}break;
		case FIT_CALIB_IN_LINE_MODE:
		{
			coord_ = FIT_CALIB_BALL_COORD;
			degree_[FIT_CALIB_PTZ_PAN] = 3;
			degree_[FIT_CALIB_PTZ_TILT] = 3;
			degree_[FIT_CALIB_PTZ_ZOOM] = 3;
		}break;
	}
}

int fit_calib::get_fit_mode()
{
	return fit_mode_;
}


int fit_calib::compute(std::vector<pair<struct fit_calib_ptz_pose, struct fit_calib_detect_pose> > &samples)
{
	int ret;
	
	if (samples.size() < 3)
		return -1;
	
	vector<pair<float, float> > samps[FIT_CALIB_PTZ_MAX_CHANNEL];
	for (int i = 0; i < samples.size(); i++)
	{
		struct fit_calib_ptz_pose &ptz_pose = samples[i].first;
		struct fit_calib_detect_pose &detect_pose = samples[i].second;
		for (int j = 0; j < FIT_CALIB_PTZ_MAX_CHANNEL; j++)
		{
			samps[j].push_back(make_pair(ptz_pose.val[j], detect_pose.val[j]));
		}	
	}	

	for (int i = 0; i < FIT_CALIB_PTZ_MAX_CHANNEL; i++)
	{
		ret = fits_[i]->compute(samps[i], degree_[i]);
		if (ret < 0)
			return -1;
	}	
	
	return 0;
}

void fit_calib::calc_ptz_pose(struct stereo_detect_box &detect_box, struct fit_calib_ptz_pose &ptz_pose)
{
	struct fit_calib_detect_pose detect_pose = {0};
	sample_detect_pose(detect_box, detect_pose);
	for (int i = 0; i < FIT_CALIB_PTZ_MAX_CHANNEL; i++)
	{
		ptz_pose.val[i] = (int)fits_[i]->calc_fit(detect_pose.val[i]);
	}	
}

void fit_calib::sample_ptz_pose(int pan, int tilt, int zoom, struct fit_calib_ptz_pose &ptz_pose)
{
	ptz_pose.val[FIT_CALIB_PTZ_PAN] = pan;
	ptz_pose.val[FIT_CALIB_PTZ_TILT] = tilt;
	ptz_pose.val[FIT_CALIB_PTZ_ZOOM] = zoom;
}

void fit_calib::sample_detect_pose(struct stereo_detect_box &detect_box, struct fit_calib_detect_pose &detect_pose)
{
	switch(coord_)
	{
		case FIT_CALIB_GRAPH_COORD:
		{
			detect_pose.val[FIT_CALIB_PTZ_PAN] = (float)detect_box.box_x;
			detect_pose.val[FIT_CALIB_PTZ_TILT] = (float)detect_box.box_y;
			detect_pose.val[FIT_CALIB_PTZ_ZOOM] = (float)detect_box.box_w;
		}break; 
		case FIT_CALIB_DEPTH_COORD:
		{
			detect_pose.val[FIT_CALIB_PTZ_PAN] = (float)detect_box.x;
			detect_pose.val[FIT_CALIB_PTZ_TILT] = (float)detect_box.y;
			detect_pose.val[FIT_CALIB_PTZ_ZOOM] = (float)detect_box.d;
		}break;
		case FIT_CALIB_CAMEAR_COORD:
		{
			detect_pose.val[FIT_CALIB_PTZ_PAN] = (float)detect_box.xcm;
			detect_pose.val[FIT_CALIB_PTZ_TILT] = (float)detect_box.ycm;
			detect_pose.val[FIT_CALIB_PTZ_ZOOM] = (float)detect_box.zcm;
		}break;
		case FIT_CALIB_ROOM_COORD:
		{
			detect_pose.val[FIT_CALIB_PTZ_PAN] = (float)detect_box.xtcm;
			detect_pose.val[FIT_CALIB_PTZ_TILT] = (float)detect_box.ytcm;
			detect_pose.val[FIT_CALIB_PTZ_ZOOM] = (float)detect_box.ztcm;
		}break;
		case FIT_CALIB_BALL_COORD:
		{
			detect_pose.val[FIT_CALIB_PTZ_PAN] = (float)detect_box.xa;
			detect_pose.val[FIT_CALIB_PTZ_TILT] = (float)detect_box.ya;
			detect_pose.val[FIT_CALIB_PTZ_ZOOM] = (float)detect_box.r;
		}break;
		default:
			break;
	}
}

int fit_calib::set_paras(string &value)
{
	try {
		Json::Reader reader;
		Json::Value jroot;
		Json::Value jpans;
		Json::Value jtilts;
		Json::Value jzooms;
		 
		if (!reader.parse(value, jroot))
			return -1;
		
		if (jroot["pans"].empty())
			return -1; 
		
		if (jroot["tilts"].empty())
			return -1;
		
		if (jroot["zooms"].empty())
			return -1;
		
		jpans = jroot["pans"];
		jtilts = jroot["tilts"];
		jzooms = jroot["zooms"];
		
		if (jpans.size() != (degree_[FIT_CALIB_PTZ_PAN] + 1))
			return -1;
		
		if (jtilts.size() != (degree_[FIT_CALIB_PTZ_TILT] + 1))
			return -1;
		
		if (jzooms.size() != (degree_[FIT_CALIB_PTZ_ZOOM] + 1))
			return -1;
		vector<double> para;
		para.resize(jpans.size());
		for (int i = 0; i < jpans.size(); i++)
		{
			para[i] = jpans[i].asDouble();
		}	
		fits_[FIT_CALIB_PTZ_PAN]->set_paras(para);
		
		para.resize(jtilts.size());
		for (int i = 0; i < jtilts.size(); i++)
		{
			para[i] = jtilts[i].asDouble();
		}	
		fits_[FIT_CALIB_PTZ_TILT]->set_paras(para);
		
		para.resize(jzooms.size());
		for (int i = 0; i < jzooms.size(); i++)
		{
			para[i] = jzooms[i].asDouble();
		}	
		fits_[FIT_CALIB_PTZ_ZOOM]->set_paras(para);
		
	} catch(std::exception &ex)
    {
        printf( "jsoncpp struct error: %s.\n", ex.what());
        return -1;
	} 
	return 0;
}

int fit_calib::get_paras(string &value)
{
	try {
		Json::Value jroot;
		Json::Value jpans;
		Json::Value jtilts;
		Json::Value jzooms;
		
		vector<double> pan = fits_[FIT_CALIB_PTZ_PAN]->get_paras();
		for (int i = 0; i < pan.size(); i++)
		{
			Json::Value jpan;
			jpan["pan"] = pan[i];
			jpans.append(jpan);
		}	
		
		vector<double> tilt = fits_[FIT_CALIB_PTZ_TILT]->get_paras();
		for (int i = 0; i < tilt.size(); i++)
		{
			Json::Value jtilt;
			jtilt["tilt"] = tilt[i];
			jtilts.append(jtilt);
		}	
		
		vector<double> zoom = fits_[FIT_CALIB_PTZ_ZOOM]->get_paras();
		for (int i = 0; i < zoom.size(); i++)
		{
			Json::Value jzoom;
			jzoom["zoom"] = zoom[i];
			jzooms.append(jzoom);
		}	
		
		jroot["pans"] = jpans;
		jroot["tilts"] = jtilts;
		jroot["zooms"] = jzooms;
	//	value = jroot.toStyledString();
		Json::StreamWriterBuilder builder;
		builder["indentation"] = "";
		value = Json::writeString(builder, jroot);
	//	cout << value << endl;
	}
	catch(std::exception &ex)
    {
        printf( "jsoncpp struct error: %s.\n", ex.what());
        return -1;
	}
	return 0;
}









