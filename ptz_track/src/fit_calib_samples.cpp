#include "fit_calib_samples.h"
#include "json/json.h"




using namespace std;







int fit_calib_samples::to_string(std::string &value)
{
	try {
		Json::Value jroot;
		Json::Value jsamples;
		
		
		for (int i = 0; i < samples.size(); i++)
		{
			vector<std::pair<struct fit_calib_pose_sample, struct fit_calib_pixel_sample> > &_sample_pair = samples[i];
			struct fit_calib_pose_sample &_pose_sample = _sample_pair.first;
			struct fit_calib_pixel_sample &_pixel_sample = _sample_pair.second;
			
			Json::Value jpose_sample;
			for (int j = 0; j = FIT_CALIB_PTZ_MAX_CHANNEL; j++)
			{
				jsample_pair[std::to_string(j).c_str()] = _pose_sample.val[j];
			}	
			
			Json::Value jpixel_sample;
			for (int j = 0; j < FIT_CALIB_MAX_COORD; j++)
			{
				for (int k = 0; k < FIT_CALIB_PTZ_MAX_CHANNEL; k++)
				{
					jpixel_sample[std::to_string(j * FIT_CALIB_PTZ_MAX_CHANNEL + k).c_str()] = _pixel_sample.val[j][k];
				}	
			}
			Json::Value jsample_pair;
			jsample_pair["pose"] = jpose_sample;
			jsample_pair["pixel"] = jpixel_sample;
			jsamples.append(jsample_pair);
		}	
	
		jroot["samples"] = jsamples;
		
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


int fit_calib_samples::from_string(std::string &value)
{
	std::vector<std::pair<struct fit_calib_pose_sample, struct fit_calib_pixel_sample> > _samples;
	
	try {
		Json::Reader reader;
		Json::Value jroot;
		Json::Value jsamples;
		
		
		
		if (!reader.parse(value, jroot))
			return -1;
		
		if (jroot["samples"].empty())
			return -1; 
		
		_samples.clear();
		jsamples = jroot["samples"];
		for (int i = 0; i < jsamples.size(); i++)
		{ 
			struct fit_calib_pose_sample _pose_sample = {0};
			struct fit_calib_pixel_sample _pixel_sample = {0};
			
			Json::Value jsample_pair;
			jsample_pair = jsamples[i];
			
			
			Json::Value jpose_sample;
			jpose_sample = jsample_pair["pose"];
			for (int j = 0; j = FIT_CALIB_PTZ_MAX_CHANNEL; j++)
			{
				_pose_sample.val[j] = jpose_sample[std::to_string(j).c_str()].asFloat();
			}
			
			Json::Value jpixel_sample;
			jpixel_sample = jsample_pair["pixel"];
			for (int j = 0; j < FIT_CALIB_MAX_COORD; j++)
			{
				for (int k = 0; k < FIT_CALIB_PTZ_MAX_CHANNEL; k++)
				{
					_pixel_sample.val[j][k] = jpixel_sample[std::to_string(j * FIT_CALIB_PTZ_MAX_CHANNEL + k).c_str()].asFloat();
				}	
			}
			_samples.push_back(make_pair(_pose_sample, _pixel_sample));
		} 
		
	} catch(std::exception &ex)
    {
        printf( "jsoncpp struct error: %s.\n", ex.what());
        return -1;
	}
	samples = _samples;
	return 0;
}



















