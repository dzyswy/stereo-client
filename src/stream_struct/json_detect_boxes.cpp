#include "json/json.h"
#include "json_detect_boxes.h"

using namespace std;

void json_detect_boxes::to_struct(vector<struct stereo_detect_box> &value)
{
	value = detect_boxes;
}

void json_detect_boxes::from_struct(vector<struct stereo_detect_box> &value)
{
	detect_boxes = value;
}

int json_detect_boxes::to_string(std::string &value)
{
	try {
		Json::Value jroot;
		Json::Value jdetect_boxes;
		Json::Value jdetect_box;
		
		for (int i = 0; i < detect_boxes.size(); i++)
		{
			jdetect_box["id"] = detect_boxes[i].id;
			jdetect_box["box_x"] = detect_boxes[i].box_x;
			jdetect_box["box_y"] = detect_boxes[i].box_y;
			jdetect_box["box_w"] = detect_boxes[i].box_w;
			jdetect_box["box_h"] = detect_boxes[i].box_h;
			jdetect_box["x"] = detect_boxes[i].x;
			jdetect_box["y"] = detect_boxes[i].y;
			jdetect_box["d"] = detect_boxes[i].d;
			jdetect_box["xcm"] = detect_boxes[i].xcm;
			jdetect_box["ycm"] = detect_boxes[i].ycm;
			jdetect_box["zcm"] = detect_boxes[i].zcm;
			jdetect_box["xtcm"] = detect_boxes[i].xtcm;
			jdetect_box["ytcm"] = detect_boxes[i].ytcm;
			jdetect_box["ztcm"] = detect_boxes[i].ztcm;
			jdetect_box["xa"] = detect_boxes[i].xa;
			jdetect_box["ya"] = detect_boxes[i].ya;
			jdetect_box["r"] = detect_boxes[i].r;
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


int json_detect_boxes::from_string(std::string &value)
{
	vector<struct stereo_detect_box> _detect_boxes;
	
	try {
		Json::Reader reader;
		Json::Value jroot;
		Json::Value jdetect_boxes;
		Json::Value jdetect_box;
		
		
		if (!reader.parse(value, jroot))
			return -1;
		
		if (jroot["detect_boxes"].empty())
			return -1; 
		
		int _count = jroot["detect_boxes"].size();
		if (_count)
			_detect_boxes.resize(_count);
		else
			_detect_boxes.clear();
		jdetect_boxes = jroot["detect_boxes"];
		for (int i = 0; i < _count; i++)
		{
			jdetect_box = jdetect_boxes[i];
			_detect_boxes[i].id = jdetect_box["id"].asInt();
			_detect_boxes[i].box_x = jdetect_box["box_x"].asInt();
			_detect_boxes[i].box_y = jdetect_box["box_y"].asInt();
			_detect_boxes[i].box_w = jdetect_box["box_w"].asInt();
			_detect_boxes[i].box_h = jdetect_box["box_h"].asInt();
			_detect_boxes[i].x = jdetect_box["x"].asInt();
			_detect_boxes[i].y = jdetect_box["y"].asInt();
			_detect_boxes[i].d = jdetect_box["d"].asInt();
			_detect_boxes[i].xcm = jdetect_box["xcm"].asInt();
			_detect_boxes[i].ycm = jdetect_box["ycm"].asInt();
			_detect_boxes[i].zcm = jdetect_box["zcm"].asInt();
			_detect_boxes[i].xtcm = jdetect_box["xtcm"].asInt();
			_detect_boxes[i].ytcm = jdetect_box["ytcm"].asInt();
			_detect_boxes[i].ztcm = jdetect_box["ztcm"].asInt();
			_detect_boxes[i].xa = jdetect_box["xa"].asFloat();
			_detect_boxes[i].ya = jdetect_box["ya"].asFloat();
			_detect_boxes[i].r = jdetect_box["r"].asFloat();
		} 
		
	} catch(std::exception &ex)
    {
        printf( "jsoncpp struct error: %s.\n", ex.what());
        return -1;
	}
	detect_boxes = _detect_boxes;
	return 0;
}













