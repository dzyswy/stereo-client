#include "json_float_pairs.h"
#include "json/json.h"


using namespace std;

json_float_pairs::json_float_pairs()
{
	
}

json_float_pairs::json_float_pairs(std::vector<std::pair<float, float> > &value)
{
	data_ = value;
}

std::vector<std::pair<float, float> > json_float_pairs::to_struct()
{
	return data_;
}


int json_float_pairs::from_string(std::string &value)
{
	std::vector<std::pair<float, float> > data;
	try {
		Json::Reader reader;
		Json::Value jroot;
		Json::Value jdata;
		Json::Value jpair;
		
		
		if (!reader.parse(value, jroot))
			return -1;
		
		if (jroot["data"].empty())
			return -1; 
		
		jdata = jroot["data"];
		

		data.clear();
		for (int i = 0; i < jdata.size(); i++)
		{
			jpair = jdata[i];
			float _first = jpair["first"].asFloat();
			float _second = jpair["second"].asFloat();
			data.push_back(std::make_pair(_first, _second)); 
		} 
		
	} catch(std::exception &ex)
    {
        printf( "jsoncpp struct error: %s.\n", ex.what());
        return -1;
	}
	data_ = data;
	return 0;
}

int json_float_pairs::to_string(std::string &value)
{
	try {
		Json::Value jroot;
		Json::Value jdata;
		Json::Value jpair;
		
		for (int i = 0; i < data_.size(); i++)
		{
			jpair["first"] = data_[i].first;
			jpair["second"] = data_[i].second;
			jdata.append(jpair);
		}	
		jroot["data"] = jdata;
		
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


















