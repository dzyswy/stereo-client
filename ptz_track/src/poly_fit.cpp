#include "poly_fit.h"
#include <cmath>
#include "linearfit.h"


using namespace std;


poly_fit::poly_fit()
{
	paras_.resize(1);
	paras_[0] = 0;
}


int poly_fit::compute(std::vector<std::pair<float, float> > &samples, int degree)
{
	int count = samples.size();
	double *first = new double [count];
	double *second = new double [count];
	
	for (int i = 0; i < count; i++)
	{
		first[i] = samples[i].first;
		second[i] = samples[i].second;
	}	
	
	
	Polynomial pf;
	pf.setAttribute(degree);
	pf.setSample(first, second, count);
	bool ret = pf.process();
	delete first;
	delete second;
	if (!ret) {
		return -1;
	}
	pf.print();
	
	paras_.resize(degree + 1);
	for (int i = 0; i < paras_.size(); i++)
	{
		paras_[i] = pf.getResult(paras_.size() - i - 1);
	}
	return 0; 
}

float poly_fit::calc_fit(float value)
{
	double sum = paras_[0];
	for (int i = 1; i < paras_.size(); i++)
	{
		sum += pow(value, i) * paras_[i];
	}	
	return (float)sum;
}










