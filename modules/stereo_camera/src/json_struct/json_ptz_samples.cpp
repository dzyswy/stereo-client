#include "json_ptz_samples.h"
#include "json/json.h"


using namespace std;

json_ptz_samples::json_ptz_samples()
{
	
}

json_ptz_samples::json_ptz_samples(std::vector<std::pair<struct stereo_ptz_pose, struct stereo_detect_box> > &value)
{
	ptz_samples_ = value;
}

std::vector<std::pair<struct stereo_ptz_pose, struct stereo_detect_box> > json_ptz_samples::to_struct()
{
	return ptz_samples_;
}


int json_ptz_samples::from_string(std::string &value)
{
	std::vector<std::pair<struct stereo_ptz_pose, struct stereo_detect_box> > ptz_samples;
	
	try {
		Json::Reader reader;
		Json::Value jroot;
		Json::Value jptz_samples;
		Json::Value jpair_sample;
		Json::Value jptz_pose;
		Json::Value jdetect_box;
		
		
		if (!reader.parse(value, jroot))
			return -1;
		
		if (jroot["ptz_samples"].empty())
			return -1; 

		jptz_samples = jroot["ptz_samples"];
		for (int i = 0; i < jptz_samples.size(); i++)
		{
			jpair_sample = jptz_samples[i];
			jptz_pose = jpair_sample["ptz_pose"];
			jdetect_box = jpair_sample["detect_box"];
			
			struct stereo_ptz_pose ptz_pose;
			ptz_pose.val[PTZ_PAN_CHANNEL] = jptz_pose["pan"].asInt();
			ptz_pose.val[PTZ_TILT_CHANNEL] = jptz_pose["tilt"].asInt();
			ptz_pose.val[PTZ_ZOOM_CHANNEL] = jptz_pose["zoom"].asInt();
			
			struct stereo_detect_box detect_box;
			detect_box.id = jdetect_box["id"].asInt();
			detect_box.box_x = jdetect_box["box_x"].asInt();
			detect_box.box_y = jdetect_box["box_y"].asInt();
			detect_box.box_w = jdetect_box["box_w"].asInt();
			detect_box.box_h = jdetect_box["box_h"].asInt();
			detect_box.x = jdetect_box["x"].asInt();
			detect_box.y = jdetect_box["y"].asInt();
			detect_box.d = jdetect_box["d"].asInt();
			detect_box.xcm = jdetect_box["xcm"].asInt();
			detect_box.ycm = jdetect_box["ycm"].asInt();
			detect_box.zcm = jdetect_box["zcm"].asInt();
			detect_box.xtcm = jdetect_box["xtcm"].asInt();
			detect_box.ytcm = jdetect_box["ytcm"].asInt();
			detect_box.ztcm = jdetect_box["ztcm"].asInt();
			detect_box.xa = jdetect_box["xa"].asFloat();
			detect_box.ya = jdetect_box["ya"].asFloat();
			detect_box.r = jdetect_box["r"].asFloat();
			detect_box.pan = jdetect_box["pan"].asInt();
			detect_box.tilt = jdetect_box["tilt"].asInt();
			detect_box.zoom = jdetect_box["zoom"].asInt();
			
			
			
			ptz_samples.push_back(make_pair(ptz_pose, detect_box));
		} 
		
	} catch(std::exception &ex)
    {
        printf( "jsoncpp struct error: %s.\n", ex.what());
        return -1;
	}
	ptz_samples_ = ptz_samples;
	return 0;
}

int json_ptz_samples::to_string(std::string &value)
{
	try {
		Json::Value jroot;
		Json::Value jptz_samples;
		Json::Value jpair_sample;
		Json::Value jptz_pose;
		Json::Value jdetect_box;
		
		for (int i = 0; i < ptz_samples_.size(); i++)
		{
			std::pair<struct stereo_ptz_pose, struct stereo_detect_box> &pair_sample = ptz_samples_[i];
			
			struct stereo_ptz_pose ptz_pose = pair_sample.first;
			jptz_pose["pan"] = ptz_pose.val[PTZ_PAN_CHANNEL];
			jptz_pose["tilt"] = ptz_pose.val[PTZ_TILT_CHANNEL];
			jptz_pose["zoom"] = ptz_pose.val[PTZ_ZOOM_CHANNEL];
			
			struct stereo_detect_box detect_box = pair_sample.second;
			jdetect_box["id"] = detect_box.id;
			jdetect_box["box_x"] = detect_box.box_x;
			jdetect_box["box_y"] = detect_box.box_y;
			jdetect_box["box_w"] = detect_box.box_w;
			jdetect_box["box_h"] = detect_box.box_h;
			jdetect_box["x"] = detect_box.x;
			jdetect_box["y"] = detect_box.y;
			jdetect_box["d"] = detect_box.d;
			jdetect_box["xcm"] = detect_box.xcm;
			jdetect_box["ycm"] = detect_box.ycm;
			jdetect_box["zcm"] = detect_box.zcm;
			jdetect_box["xtcm"] = detect_box.xtcm;
			jdetect_box["ytcm"] = detect_box.ytcm;
			jdetect_box["ztcm"] = detect_box.ztcm;
			jdetect_box["xa"] = detect_box.xa;
			jdetect_box["ya"] = detect_box.ya;
			jdetect_box["r"] = detect_box.r;
			jdetect_box["pan"] = detect_box.pan;
			jdetect_box["tilt"] = detect_box.tilt;
			jdetect_box["zoom"] = detect_box.zoom;
			
			jpair_sample["ptz_pose"] = jptz_pose;
			jpair_sample["detect_box"] = jdetect_box;
			
			jptz_samples.append(jpair_sample);
		}	
		jroot["ptz_samples"] = jptz_samples;
		
	//	value = jroot.toStyledString();
		Json::StreamWriterBuilder builder;
		builder["indentation"] = "";
		value = Json::writeString(builder, jroot);
	}
	catch(std::exception &ex)
    {
        printf( "jsoncpp struct error: %s.\n", ex.what());
        return -1;
	}
	return 0;
}


















