#include <iostream>
#include <stdlib.h>
#include <time.h> 
#include <vector>
#include <cmath>

#include "poly_fit.h"



using namespace std;


float calc_fit(vector<double> &para, float value)
{
	double sum = para[0];
	for (int i = 1; i < para.size(); i++)
	{
		sum += pow(value, i) * para[i];
	}

	return (float)sum;
}


int main(int argc, char *argv[])
{
	int ret;
	if (argc < 3)
	{
		printf("usage: %s samples a0 a1 a2 ... an\n", argv[0]);
		printf("a0 + a1*x + a2*x^2 + ... + an*x^n \n");
		return -1;
	}	
	
	srand((unsigned)time(NULL)); 
	
	int sample_size = atoi(argv[1]);
	int degree = argc - 3;
	
	vector<double> para_val;
	for (int i = 0; i <= degree; i++)
	{
		para_val.push_back(atof(argv[i + 2]));
	}

	printf("samples:\n");
	vector<pair<float, float> > samples; 
	for (int i = -sample_size; i <= sample_size; i++)
	{
		float x = (float)i;
		float y = calc_fit(para_val, x);
		float rx = x + rand() / double(RAND_MAX) - 0.5;
		float ry = y + rand() / double(RAND_MAX) - 0.5;
		
		printf("s%d: (%f, %f) (%f, %f)\n", i, x, y, rx, ry);
		
		samples.push_back(make_pair(rx, ry));
	}	
	
	poly_fit xfit;
	ret = xfit.compute(samples, degree);
	if (ret < 0) {
		printf("fit error!\n");
		return -1;
	}
	
	vector<double> para_fit = xfit.get_paras();
	
	printf("\npara:\n");
	for (int i = 0; i < para_fit.size(); i++)
	{
		printf("a%d: %f %f\n", i, para_val[i], para_fit[i]);
	}	
	
	printf("\nfit:\n");
	int k = 0;
	for (int i = -sample_size; i <= sample_size; i++)
	{
		float x = (float)i;
		float y = calc_fit(para_val, x);
		float rx = (float)i;
		float ry = xfit.calc_fit(rx);
		
		printf("s%d: (%f, %f) (%f %f) (%f, %f)\n", i, x, y, samples[k].first, samples[k].second, rx, ry);
		k++;
	}	
	
	return 0;
}


















