#include "json_detect_boxes.h"
#include "json/json.h"


using namespace std;

json_detect_boxes::json_detect_boxes()
{
	
}

json_detect_boxes::json_detect_boxes(std::vector<struct stereo_detect_box> &value)
{
	detect_boxes_ = value;
}

std::vector<struct stereo_detect_box> json_detect_boxes::to_struct()
{
	return detect_boxes_;
}


int json_detect_boxes::from_string(std::string &value)
{
	vector<struct stereo_detect_box> detect_boxes;
	
	try {
		Json::Reader reader;
		Json::Value jroot;
		Json::Value jdetect_boxes;
		Json::Value jdetect_box;
		
		
		if (!reader.parse(value, jroot))
			return -1;
		
		if (jroot["detect_boxes"].empty())
			return -1; 

		jdetect_boxes = jroot["detect_boxes"];
		for (int i = 0; i < jdetect_boxes.size(); i++)
		{
			jdetect_box = jdetect_boxes[i];
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
			detect_boxes.push_back(detect_box);
		} 
		
	} catch(std::exception &ex)
    {
        printf( "jsoncpp struct error: %s.\n", ex.what());
        return -1;
	}
	detect_boxes_ = detect_boxes;
	return 0;
}

int json_detect_boxes::to_string(std::string &value)
{
	try {
		Json::Value jroot;
		Json::Value jdetect_boxes;
		Json::Value jdetect_box;
		
		for (int i = 0; i < detect_boxes_.size(); i++)
		{
			struct stereo_detect_box &detect_box = detect_boxes_[i];
			
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
			jdetect_boxes.append(jdetect_box);
		}	
		jroot["detect_boxes"] = jdetect_boxes;
		
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


















