#ifndef __POLY_FIT_H
#define __POLY_FIT_H


#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <ctime>

class poly_fit
{
public:
	poly_fit(int debug = 0);	
	int compute(std::vector<std::pair<float, float> > &samples, int degree = 1);
	float calc_fit(float value);
	 
	
protected:
	int debug_;
	std::vector<double> paras_;
	
	
public:
	void set_paras(std::vector<double> &value)
	{
		if (value.size() < 2)
			return;
		paras_ = value;
		
		if (debug_)
		{
			printf("\npara:\n");
			for (int i = 0; i < paras_.size(); i++)
			{
				printf("a%d: %f\n", i, paras_[i]);
			}	
		}	
	}
	
	std::vector<double> &get_paras()
	{
		return paras_;
	}
};



















#endif

