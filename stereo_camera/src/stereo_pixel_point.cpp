#include <iostream>
#include "json/json.h"
#include "stereo_pixel_point.h"

using namespace std;



int stereo_pixel_point::to_string(std::string &value)
{
	try {
		Json::Value jroot;
		Json::Value jpixel_point;

		jpixel_point["x"] = x;
		jpixel_point["y"] = y;
		jpixel_point["d"] = d;
		jpixel_point["xcm"] = xcm;
		jpixel_point["ycm"] = ycm;
		jpixel_point["zcm"] = zcm;
		jpixel_point["xtcm"] = xtcm;
		jpixel_point["ytcm"] = ytcm;
		jpixel_point["ztcm"] = ztcm;
		jpixel_point["xa"] = xa;
		jpixel_point["ya"] = ya;
		jpixel_point["r"] = r;
		
		jroot["pixel_point"] = jpixel_point;
		
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


int stereo_pixel_point::from_string(std::string &value)
{
	float _roll;
	float _pitch;
	
	try {
		Json::Reader reader;
		Json::Value jroot;
		Json::Value jpixel_point;
		
		if (!reader.parse(value, jroot))
			return -1;
		
		if (jroot["pixel_point"].empty())
			return -1;

		jpixel_point = jroot["pixel_point"];

		x = jpixel_point["x"].asInt();
		y = jpixel_point["y"].asInt();
		d = jpixel_point["d"].asInt();
		xcm = jpixel_point["xcm"].asInt();
		ycm = jpixel_point["ycm"].asInt();
		zcm = jpixel_point["zcm"].asInt();
		xtcm = jpixel_point["xtcm"].asInt();
		ytcm = jpixel_point["ytcm"].asInt();
		ztcm = jpixel_point["ztcm"].asInt();
		xa = jpixel_point["xa"].asDouble();
		ya = jpixel_point["ya"].asDouble();
		r = jpixel_point["r"].asDouble();
		
	} catch(std::exception &ex)
    {
        printf( "jsoncpp struct error: %s.\n", ex.what());
        return -1;
	}

	return 0;
}













