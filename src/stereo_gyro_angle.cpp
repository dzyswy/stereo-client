#include <iostream>
#include "json/json.h"
#include "stereo_gyro_angle.h"

using namespace std;



int stereo_gyro_angle::to_string(std::string &value)
{
	try {
		Json::Value jroot;
		Json::Value jgyro_angle;

		jgyro_angle["roll"] = roll;
		jgyro_angle["pitch"] = pitch;
		
		jroot["gyro_angle"] = jgyro_angle;
		
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


int stereo_gyro_angle::from_string(std::string &value)
{
	float _roll;
	float _pitch;
	
	try {
		Json::Reader reader;
		Json::Value jroot;
		Json::Value jgyro_angle;
		
		if (!reader.parse(value, jroot))
			return -1;
		
		if (jroot["gyro_angle"].empty())
			return -1;

		jgyro_angle = jroot["gyro_angle"];
		if (jgyro_angle["roll"].empty())
			return -1;
		
		if (jgyro_angle["pitch"].empty())
			return -1;

		_roll = jgyro_angle["roll"].asFloat();
		_pitch = jgyro_angle["pitch"].asFloat();
		
	} catch(std::exception &ex)
    {
        printf( "jsoncpp struct error: %s.\n", ex.what());
        return -1;
	}

	roll = _roll;
	pitch = _pitch;
	return 0;
}













