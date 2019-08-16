#include "json/json.h"
#include "json_gyro_angle.h"

json_gyro_angle::json_gyro_angle(struct stereo_gyro_angle &value)
{
	gyro_angle = value;
}

int json_gyro_angle::to_string(std::string &value)
{
	try {
		Json::Value jroot;
		Json::Value jgyro_angle;

		jgyro_angle["roll"] = gyro_angle.roll;
		jgyro_angle["pitch"] = gyro_angle.pitch;
		
		jroot["gyro_angle"] = jgyro_angle;
		
		value = jroot.toStyledString();
	}
	catch(std::exception &ex)
    {
        printf( "jsoncpp struct error: %s.\n", ex.what());
        return -1;
	}
	return 0;
}


int json_gyro_angle::from_string(std::string &value)
{
	struct stereo_gyro_angle _gyro_angle;
	
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

		_gyro_angle.roll = jgyro_angle["roll"].asFloat();
		_gyro_angle.pitch = jgyro_angle["pitch"].asFloat();
		
	} catch(std::exception &ex)
    {
        printf( "jsoncpp struct error: %s.\n", ex.what());
        return -1;
	}

	gyro_angle = _gyro_angle;
	return 0;
}













