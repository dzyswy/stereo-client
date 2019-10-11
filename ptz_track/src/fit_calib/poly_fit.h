#ifndef __POLY_FIT_H
#define __POLY_FIT_H


#include <iostream>
#include <vector>
#include <string>

class poly_fit
{
public:
	poly_fit();	
	int compute(std::vector<std::pair<float, float> > &samples, int degree = 1);
	float calc_fit(float value);
	 
	
protected:
	std::vector<double> paras_;
	
	
public:
	void set_paras(std::vector<double> &value)
	{
		paras_ = value;
	}
	
	std::vector<double> &get_paras()
	{
		return paras_;
	}
};



















#endif

