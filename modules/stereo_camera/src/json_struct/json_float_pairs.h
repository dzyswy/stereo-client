#ifndef __JSON_FLOAT_PAIRS_H
#define __JSON_FLOAT_PAIRS_H


#include "stereo_struct.h"




class json_float_pairs
{
public:
	json_float_pairs();
	json_float_pairs(std::vector<std::pair<float, float> > &value);
	std::vector<std::pair<float, float> > to_struct();
	
	int from_string(std::string &value);
	int to_string(std::string &value);

protected:
	std::vector<std::pair<float, float> > data_;
};
















#endif



