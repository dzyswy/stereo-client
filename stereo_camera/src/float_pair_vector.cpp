#include "float_pair_vector.h"
#include "json/json.h"


using namespace std;







int float_pair_vector::to_string(std::string &value)
{
	try {
		Json::Value jroot;
		Json::Value jdata;
		Json::Value jpair;
		
		for (int i = 0; i < data.size(); i++)
		{
			jpair["first"] = data[i].first;
			jpair["second"] = data[i].second;
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


int float_pair_vector::from_string(std::string &value)
{
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
	return 0;
}













