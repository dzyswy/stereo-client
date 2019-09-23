#ifndef __FLOAT_PAIR_VECTOR_H
#define __FLOAT_PAIR_VECTOR_H


#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <exception>
#include <string>
#include <vector>



struct float_pair_vector
{
	std::vector<std::pair<float, float> > data;
	
	int to_string(std::string &value);
	int from_string(std::string &value);
};











#endif



















