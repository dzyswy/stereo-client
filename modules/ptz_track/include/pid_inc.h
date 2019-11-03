#ifndef __PID_INC_H
#define __PID_INC_H


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

class pid_inc
{
public:
	pid_inc(int debug = 0);
	float compute(float err);
	void pid_reset(float err)
	{
		err_1 = err;
		err_2 = err;
	}
	void set_kp(float value)
	{
		kp = value;
	}

	void set_ki(float value)
	{
		ki = value;
	}

	void set_kd(float value)
	{
		kd = value;
	}

	void set_dead_zone(float value)
	{
		dead_zone = value;
	}

	void set_max_limit(float value)
	{
		max_limit = value;
	}

	
	float get_kp()
	{
		return kp;
	}
	float get_ki()
	{
		return ki;
	}

	float get_kd()
	{
		return kd;
	}

	float get_dead_zone()
	{
		return dead_zone;
	}
	float get_max_limit()
	{
		return max_limit;
	}
	
private:
	int debug_;
	float kp;
	float ki;
	float kd;
	float dead_zone;
	float max_limit;
	
	float err_1;
	float err_2;
};












#endif
