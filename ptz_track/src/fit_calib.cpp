#include "fit_calib.h"
#include "linearfit.h"
#include "polynomialfit.h"
#include "ptz_ctl_visca.h"
#include "stereo_camera.h"
#include "json/json.h"





using namespace std;


fit_calib::fit_calib()
{
	fit_mode_ = FIT_CALIB_CLOSE_TO_MODE;
	coord_ = FIT_CALIB_BALL_COORD;
}

void fit_calib::set_fit_mode(int value)
{
	fit_mode_ = value;
	
	switch(fit_mode_)
	{
		case FIT_CALIB_CLOSE_TO_MODE:
		{
			degree_[FIT_CALIB_PTZ_PAN] = 1;
			degree_[FIT_CALIB_PTZ_TILT] = 1;
			degree_[FIT_CALIB_PTZ_ZOOM] = 3;
			
		}break;
		case FIT_CALIB_IN_LINE_MODE:
		{
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


int fit_calib::set_sample(int pan, int tilt, int zoom, struct stereo_pixel_point &pixel, int index)
{
	int sample_size = get_samples_size();
	if ((index < 0) || (index >= sample_size))
		return -1;
	
	struct fit_calib_ptz_pose ptz_pose = {0};
	struct fit_calib_stereo_pixel stereo_pixel = {0};
	
	ptz_pose.val[FIT_CALIB_PTZ_PAN] = pan;
	ptz_pose.val[FIT_CALIB_PTZ_TILT] = tilt;
	ptz_pose.val[FIT_CALIB_PTZ_ZOOM] = zoom;
	
	to_stereo_pixel(pixel, stereo_pixel);
	
	for (int i = 0; i < FIT_CALIB_PTZ_MAX_CHANNEL; i++)
	{
		samples_[i][index] = make_pair(ptz_pose.val[i], stereo_pixel.val[i]);
	}	
	 
	return 0;
}

int fit_calib::get_sample(struct fit_calib_ptz_pose &ptz_pose, struct fit_calib_stereo_pixel &stereo_pixel, int index);
{
	int sample_size = get_samples_size();
	if ((index < 0) || (index >= sample_size))
		return -1;
	
	for (int i = 0; i < FIT_CALIB_PTZ_MAX_CHANNEL; i++)
	{
		ptz_pose.val[i] = samples_[i][index].first;
		stereo_pixel.val[i] = samples_[i][index].second; 
	}	 
	
	return 0;
}	



void fit_calib::clear_samples()
{
	for (int i = 0; i < FIT_CALIB_PTZ_MAX_CHANNEL; i++)
	{
		samples_[i].clear();
	}	
}

int fit_calib::set_samples_size(int value)
{
	for (int i = 0; i < FIT_CALIB_PTZ_MAX_CHANNEL; i++)
	{
		samples_[i].resize(value);
	}
}

int fit_calib::get_samples_size()
{
	return samples_[0].size();
}

int fit_calib::compute()
{
	int ret;
	for (int i = 0; i < FIT_CALIB_PTZ_MAX_CHANNEL; i++)
	{
		ret = compute(samples_[i], degree_[i]);
		if (ret < 0)
			return -1
	}	
	
	return 0;
}

void fit_calib::to_ptz(struct stereo_pixel_point &pixel, int &pan, int &tilt, int &zoom)
{
	struct fit_calib_stereo_pixel stereo_pixel = {0};
	to_stereo_pixel(pixel, stereo_pixel);
	pan = (int)fits_[FIT_CALIB_PTZ_PAN].calc_fit(stereo_pixel.val[FIT_CALIB_PTZ_PAN]);
	tilt = (int)fits_[FIT_CALIB_PTZ_TILT].calc_fit(stereo_pixel.val[FIT_CALIB_PTZ_TILT]);
	zoom = (int)fits_[FIT_CALIB_PTZ_ZOOM].calc_fit(stereo_pixel.val[FIT_CALIB_PTZ_ZOOM]);
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
		para.resize(jpans.size())
		for (int i = 0; i < jpans.size(); i++)
		{
			para[i] = jpans[i].asDouble();
		}	
		fits_[FIT_CALIB_PTZ_PAN].set_paras(para);
		
		para.resize(jtilts.size())
		for (int i = 0; i < jtilts.size(); i++)
		{
			para[i] = jtilts[i].asDouble();
		}	
		fits_[FIT_CALIB_PTZ_TILT].set_paras(para);
		
		para.resize(jzooms.size())
		for (int i = 0; i < jzooms.size(); i++)
		{
			para[i] = jzooms[i].asDouble();
		}	
		fits_[FIT_CALIB_PTZ_ZOOM].set_paras(para);
		
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
		
		vector<double> pan = paras_[FIT_CALIB_PTZ_PAN].get_paras();
		for (int i = 0; i < pan.size(); i++)
		{
			Json::Value jpan;
			jpan["pan"] = pan[i];
			jpans.append(jpan);
		}	
		
		vector<double> tilt = paras_[FIT_CALIB_PTZ_TILT].get_paras();
		for (int i = 0; i < tilt.size(); i++)
		{
			Json::Value jtilt;
			jtilt["tilt"] = tilt[i];
			jtilts.append(jtilt);
		}	
		
		vector<double> zoom = paras_[FIT_CALIB_PTZ_ZOOM].get_paras();
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

void fit_calib::to_stereo_pixel(struct stereo_pixel_point &pixel, struct fit_calib_stereo_pixel &stereo_pixel)
{
	switch(coord_)
	{
		case FIT_CALIB_GRAPH_COORD:
		{
			stereo_pixel.val[FIT_CALIB_PTZ_PAN] = (float)pixel.x;
			stereo_pixel.val[FIT_CALIB_PTZ_TILT] = (float)pixel.y;
			stereo_pixel.val[FIT_CALIB_PTZ_ZOOM] = (float)pixel.d;
		}break;
		case FIT_CALIB_CAMEAR_COORD:
		{
			stereo_pixel.val[FIT_CALIB_PTZ_PAN] = (float)pixel.xcm;
			stereo_pixel.val[FIT_CALIB_PTZ_TILT] = (float)pixel.ycm;
			stereo_pixel.val[FIT_CALIB_PTZ_ZOOM] = (float)pixel.zcm;
		}break;
		case FIT_CALIB_ROOM_COORD:
		{
			stereo_pixel.val[FIT_CALIB_PTZ_PAN] = (float)pixel.xtcm;
			stereo_pixel.val[FIT_CALIB_PTZ_TILT] = (float)pixel.ytcm;
			stereo_pixel.val[FIT_CALIB_PTZ_ZOOM] = (float)pixel.ztcm;
		}break;
		case FIT_CALIB_BALL_COORD:
		{
			stereo_pixel.val[FIT_CALIB_PTZ_PAN] = (float)pixel.xa;
			stereo_pixel.val[FIT_CALIB_PTZ_TILT] = (float)pixel.ya;
			stereo_pixel.val[FIT_CALIB_PTZ_ZOOM] = (float)pixel.r;
		}break;
		default:
			break;
	}
}







