#include "poly_fit.h"
#include <cmath>
#include "linearfit.h"


using namespace std;


poly_fit::poly_fit(int debug)
{
	debug_ = debug;
	paras_.resize(1);
	paras_[0] = 0;
}


int poly_fit::compute(std::vector<std::pair<float, float> > &samples, int degree)
{
	int count = samples.size();
	double *first = new double [count];
	double *second = new double [count];
	
	printf("\sample:\n");
	for (int i = 0; i < count; i++)
	{
		first[i] = samples[i].first;
		second[i] = samples[i].second;
		printf("s%d: %f %f\n", i, first[i], second[i]);
		
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
	 
	paras_.resize(degree + 1);
	for (int i = 0; i < paras_.size(); i++)
	{
		paras_[i] = pf.getResult(paras_.size() - i - 1);
	}
	
	
	if (debug_) 
	{
		pf.print();
		
		printf("\npara:\n");
		for (int i = 0; i < paras_.size(); i++)
		{
			printf("a%d: %f\n", i, paras_[i]);
		}	
		printf("\fit:\n");
		for (int i = 0; i < samples.size(); i++)
		{
			printf("o%d: %f %f %f\n", i, samples[i].first, samples[i].second, calc_fit(samples[i].first));
		}	
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










