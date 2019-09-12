#include <iostream>
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
			Json::Value jposes;
			for (int j = 0; j = FIT_CALIB_PTZ_MAX_CHANNEL; j++)
			{
				Json::Value jpose;
				jpose["pose"] = samples[i].first.val[j];
				jposes.append(jpose);
			}	
			 
			Json::Value jpixels;
			for (int j = 0; j < FIT_CALIB_MAX_COORD; j++)
			{
				for (int k = 0; k < FIT_CALIB_PTZ_MAX_CHANNEL; k++)
				{
					Json::Value jpixel;
					jpixel["pixel"] = samples[i].second.val[j][k];
					jpixels.append(jpixel);
				}	
			}
			Json::Value jsample_pair;
			jsample_pair["poses"] = jposes;
			jsample_pair["pixels"] = jpixels;
			jsamples.append(jsample_pair);
		}	
		
		jroot["samples"] = jsamples;
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


int fit_calib_samples::from_string(std::string &value)
{
	std::vector<std::pair<struct fit_calib_ptz_pose, struct fit_calib_stereo_pixel> > _samples;
	_samples.clear();
	try {
		Json::Reader reader;
		Json::Value jroot;
		Json::Value jsamples;
		 
		if (!reader.parse(value, jroot))
			return -1;
		
		if (jroot["samples"].empty())
			return -1; 
		
		jsamples = jroot["samples"];
		
		
		for (int i = 0; i < jsamples.size(); i++)
		{ 
			struct fit_calib_ptz_pose _pose_sample = {0};
			struct fit_calib_stereo_pixel _pixel_sample = {0};
			
			Json::Value jsample_pair;
			jsample_pair = jsamples[i];
			
			
			Json::Value jpose_sample;
			jpose_sample = jsample_pair["poses"];
			for (int j = 0; j = FIT_CALIB_PTZ_MAX_CHANNEL; j++)
			{
				_pose_sample.val[j] = jpose_sample[j].asFloat();
			}
			
			Json::Value jpixel_sample;
			jpixel_sample = jsample_pair["pixels"];
			for (int j = 0; j < FIT_CALIB_MAX_COORD; j++)
			{
				for (int k = 0; k < FIT_CALIB_PTZ_MAX_CHANNEL; k++)
				{
					_pixel_sample.val[j][k] = jpixel_sample[j * FIT_CALIB_PTZ_MAX_CHANNEL + k].asFloat();
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



















