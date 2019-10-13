#include <math.h>
#include <iostream>
#include "pid_inc.h"


using namespace std;


pid_inc::pid_inc(int debug)
{
	debug_ = debug;
	kp = 0;
	ki = 1;
	kd = 0;
	dead_zone = 0;
	max_limit = 0;
	pid_reset(0);
}

//err normalize to -1~0~1
float pid_inc::compute(float err)
{
	float out = 0;
	
	if (fabs(err) < dead_zone) 
	{
		pid_reset(err); 
	} 
	else 
	{
		out = kp * (err - err_1) + ki * err + kd * (err - 2 * err_1 + err_2);
			
		err_2 = err_1;
		err_1 = err;
		
		if ((max_limit > 0) && (fabs(out) > max_limit))
		{
			if (out > 0)
				out = max_limit;
			else
				out = -max_limit;
		} 	
	}
		
	if (debug_)	{
		printf("(%f %f): (%f %f %f) %f\n", dead_zone, max_limit, err, err_1, err_2, out);
	}
	
	
	return out;
}












