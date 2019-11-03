#ifndef __PTZ_TRACK_H
#define __PTZ_TRACK_H

#include <iostream>
#include <ctime>
//#include <sys/time.h>
#include <cmath>
#include <math.h>
#include <chrono>
#include <ratio>

#ifndef _WIN32
#include <unistd.h>
#include <sys/time.h>
#else 
#include <windows.h>	
#endif

#include "pid_inc.h"
#include "fit_calib.h"
#include "stereo_filter.h"
#include "ptz_ctl_visca.h"


#define PTZ_TRACK_PTZ_PAN_MASK		(1<<0)
#define PTZ_TRACK_PTZ_TILT_MASK		(1<<1)
#define PTZ_TRACK_PTZ_ZOOM_MASK		(1<<2)
#define PTZ_TRACK_PTZ_ALL_MASK		(0x7)



enum ptz_track_track_mode_type
{
	PTZ_TRACK_STOP_TRACK_MODE = 0,
	PTZ_TRACK_CONTINUOUS_TRACK_MODE = 1,
	PTZ_TRACK_LOCK_TRACK_MODE = 2,
};


enum ptz_track_trig_state_type
{
	PTZ_TRACK_TARGET_SEARCH = 0,
	PTZ_TRACK_TARGET_APPEAR = 1,
	PTZ_TRACK_TARGET_STABLE = 2,
};


struct ptz_track_ptz_speed
{
	int val[FIT_CALIB_PTZ_MAX_CHANNEL];
};
 

class ptz_track
{
public:
	ptz_track(ptz_ctl_visca *ptz, fit_calib *fit, float period = 100, int debug = 0);
	~ptz_track();
	
	void set_detect_box(struct stereo_detect_box &detect_box, int number_state = STEREO_FILTER_SINGLE_TARGET, int stable_state = 1);
	
	int pid_paras_from_string(std::string value);
	int pid_paras_to_string(std::string &value);
	
protected:
	void run();
	void stop();
	void track_process();
	void compute(struct fit_calib_ptz_pose &focus_pose, struct fit_calib_ptz_pose &current_pose, struct ptz_track_ptz_speed &ptz_speed);
	void compute(struct fit_calib_ptz_pose &focus_pose, struct fit_calib_ptz_pose &current_pose, int number_state, int pre_number_state, int stable_state);
	void pantilt_move_speed(int panSpeed, int tiltSpeed);
	void zoom_move_speed(int zoomSpeed);
	
	
protected:
	ptz_ctl_visca *ptz_;
	fit_calib *fit_;
	int debug_;

	float period_;
	int track_mode_;
	int track_mask_;
	
	int trig_state_;
	
	
	int going;
	std::mutex mux_;
	std::thread *run_thread_;
	
	struct fit_calib_ptz_pose focus_pose_;
	int number_state_;
	int stable_state_;
	
	float position_range_[FIT_CALIB_PTZ_MAX_CHANNEL][2];
	float speed_range_[FIT_CALIB_PTZ_MAX_CHANNEL][2];
	float dead_zone_[FIT_CALIB_PTZ_MAX_CHANNEL];
	 
	pid_inc *xpid_[FIT_CALIB_PTZ_MAX_CHANNEL];
	
	
	
public:

	void set_track_mode(int value)
	{
		track_mode_ = value; 
	}
	
	void set_track_mask(int value)
	{
		track_mask_ = value;
	}
	
	void set_kp(int channel, float value)
	{
		xpid_[channel]->set_kp(value);
	}
	
	void set_ki(int channel, float value)
	{
		xpid_[channel]->set_ki(value);
	}
	
	void set_kd(int channel, float value)
	{
		xpid_[channel]->set_kd(value);
	}
	
	void set_dead_zone(int channel, float value)
	{
		dead_zone_[channel] = value;
		float dead_zone = value / (position_range_[channel][1] - position_range_[channel][0]);
		xpid_[channel]->set_dead_zone(dead_zone);
	}
	
	void set_max_limit(int channel, float value)
	{
		xpid_[channel]->set_max_limit(value);
	}
	
	int get_track_mode()
	{
		return track_mode_;
	}
	
	int get_track_mask()
	{
		return track_mask_;
	}
	

	float get_kp(int channel)
	{
		return xpid_[channel]->get_kp();
	}
	
	float get_ki(int channel)
	{
		return xpid_[channel]->get_ki();
	}
	
	float get_kd(int channel)
	{
		return xpid_[channel]->get_kd();
	}
	
	float get_dead_zone(int channel)
	{
		return dead_zone_[channel];
	}
	
	float get_max_limit(int channel)
	{
		return xpid_[channel]->get_max_limit();
	}
	
	
};
















#endif


















