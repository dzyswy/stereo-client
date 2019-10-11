#include "poly_fit.h"
#include <cmath>



using namespace std;


poly_fit::poly_fit()
{
	paras_.resize(1);
	paras_[0] = 0;
}


int poly_fit::compute(std::vector<std::pair<float, float> > &samples, int degree)
{
	return 0;
}

float poly_fit::calc_fit(float value)
{
	int size = paras_.size();
	double sum = paras_[size - 1];
	for (int i = 0; i < size - 1; i++)
	{
		sum += pow(value, size - i - 1) * paras_[i];
	}	
	return (float)sum;
}










