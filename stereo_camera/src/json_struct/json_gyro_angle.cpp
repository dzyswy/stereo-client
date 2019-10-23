#include "json_gyro_angle.h"
#include "json/json.h"


using namespace std;

json_gyro_angle::json_gyro_angle()
{
	
}

json_gyro_angle::json_gyro_angle(struct stereo_gyro_angle &value)
{
	data_ = value;
}

struct stereo_gyro_angle json_gyro_angle::to_struct()
{
	return data_;
}


int json_gyro_angle::from_string(std::string &value)
{
	struct stereo_gyro_angle data;
	try {
		Json::Reader reader;
		Json::Value jroot;
		Json::Value jgyro_angle;
		
		if (!reader.parse(value, jroot))
			return -1;
		
		if (jroot["gyro_angle"].empty())
			return -1;

		jgyro_angle = jroot["gyro_angle"];
		if (jgyro_angle["xabs"].empty())
			return -1;
		if (jgyro_angle["yabs"].empty())
			return -1;
		if (jgyro_angle["zabs"].empty())
			return -1; 
		if (jgyro_angle["roll"].empty())
			return -1; 
		if (jgyro_angle["pitch"].empty())
			return -1;
		
		data.xabs = jgyro_angle["xabs"].asFloat();
		data.yabs = jgyro_angle["yabs"].asFloat();
		data.zabs = jgyro_angle["zabs"].asFloat();
		data.roll = jgyro_angle["roll"].asFloat();
		data.pitch = jgyro_angle["pitch"].asFloat();
		
	} catch(std::exception &ex)
    {
        printf( "jsoncpp struct error: %s.\n", ex.what());
        return -1;
	}
	data_ = data;
	return 0;
}

int json_gyro_angle::to_string(std::string &value)
{
	try {
		Json::Value jroot;
		Json::Value jgyro_angle;
		
		jgyro_angle["xabs"] = data_.xabs;
		jgyro_angle["yabs"] = data_.yabs;
		jgyro_angle["zabs"] = data_.zabs;
		jgyro_angle["roll"] = data_.roll;
		jgyro_angle["pitch"] = data_.pitch;
		
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


















