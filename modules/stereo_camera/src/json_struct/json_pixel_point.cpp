#include "json_pixel_point.h"
#include "json/json.h"


using namespace std;

json_pixel_point::json_pixel_point()
{
	
}

json_pixel_point::json_pixel_point(struct stereo_pixel_point &value)
{
	data_ = value;
}

struct stereo_pixel_point json_pixel_point::to_struct()
{
	return data_;
}


int json_pixel_point::from_string(std::string &value)
{
	struct stereo_pixel_point data;
	
	try {
		Json::Reader reader;
		Json::Value jroot;
		Json::Value jpixel_point;
		
		if (!reader.parse(value, jroot))
			return -1;
		
		if (jroot["pixel_point"].empty())
			return -1;

		jpixel_point = jroot["pixel_point"];

		data.x = jpixel_point["x"].asInt();
		data.y = jpixel_point["y"].asInt();
		data.d = jpixel_point["d"].asInt();
		data.xcm = jpixel_point["xcm"].asInt();
		data.ycm = jpixel_point["ycm"].asInt();
		data.zcm = jpixel_point["zcm"].asInt();
		data.xtcm = jpixel_point["xtcm"].asInt();
		data.ytcm = jpixel_point["ytcm"].asInt();
		data.ztcm = jpixel_point["ztcm"].asInt();
		data.xa = jpixel_point["xa"].asDouble();
		data.ya = jpixel_point["ya"].asDouble();
		data.r = jpixel_point["r"].asDouble();
		
	} catch(std::exception &ex)
    {
        printf( "jsoncpp struct error: %s.\n", ex.what());
        return -1;
	}
	data_ = data;
	return 0;
}

int json_pixel_point::to_string(std::string &value)
{
	try {
		Json::Value jroot;
		Json::Value jpixel_point;

		jpixel_point["x"] = data_.x;
		jpixel_point["y"] = data_.y;
		jpixel_point["d"] = data_.d;
		jpixel_point["xcm"] = data_.xcm;
		jpixel_point["ycm"] = data_.ycm;
		jpixel_point["zcm"] = data_.zcm;
		jpixel_point["xtcm"] = data_.xtcm;
		jpixel_point["ytcm"] = data_.ytcm;
		jpixel_point["ztcm"] = data_.ztcm;
		jpixel_point["xa"] = data_.xa;
		jpixel_point["ya"] = data_.ya;
		jpixel_point["r"] = data_.r;
		
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


















