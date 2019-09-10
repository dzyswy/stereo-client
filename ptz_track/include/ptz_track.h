#ifndef __PTZ_TRACK_H
#define __PTZ_TRACK_H

#include <ctime>
#include <sys/time.h>

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
	PTZ_TRACK_CONTINUOUS_TRACK_MODE = 1,
	PTZ_TRACK_LOCK_TRACK_MODE = 2,
};


enum ptz_track_track_lock_type
{
	PTZ_TRACK_TRACK_UNLOCKED = 0,
	PTZ_TRACK_TRACK_LOCKED = 1,
};



struct ptz_track_focus_pose
{
	float val[FIT_CALIB_PTZ_MAX_CHANNEL];
};

struct ptz_track_focus_pixel
{
	float val[FIT_CALIB_MAX_COORD][FIT_CALIB_PTZ_MAX_CHANNEL];
};


class ptz_track
{
public:
	ptz_track(ptz_ctl_visca *ptz, fit_calib *calib, float period = 100);
	~ptz_track();
	
	int run();
	int stop();
	
	
	void track_process();
	
protected:
	ptz_ctl_visca *ptz_;
	fit_calib *calib_;

	float period_;
	int track_mode_;
	int track_mask_;
	int track_coord_;
	int lock_time_;
	int lock_state_;
	time_t lock_timeout_;
	
	
	int going;
	std::mutex mux_;
	std::thread *run_thread_;
	
	struct ptz_track_focus_pose focus_pose_;

	
	pid_inc pid_[PTZ_TRACK_PTZ_MAX_CHANNEL];
	
	
	
public:

	void set_detect_box(struct stereo_detect_box &value);


	void set_track_mode(int value)
	{
		track_mode_ = value;
	}
	
	void set_track_mask(int value)
	{
		track_mask_ = value;
	}
	
	void set_track_coord(int value)
	{
		track_coord_ = value;
	}
	
	void set_lock_time(int value)
	{
		lock_time_ = value;
	}
	
	void set_kp(int channel, float value)
	{
		pid_[channel].set_kp(value);
	}
	
	void set_ki(int channel, float value)
	{
		pid_[channel].set_ki(value);
	}
	
	void set_kd(int channel, float value)
	{
		pid_[channel].set_kd(value);
	}
	
	void set_dead_zone(int channel, float value)
	{
		pid_[channel].set_dead_zone(value);
		dead_zone_[channel] = value;
	}
	
	void set_max_limit(int channel, float value)
	{
		pid_[channel].set_max_limit(value);
	}
	
	int get_track_mode()
	{
		return track_mode_;
	}
	
	int get_track_mask()
	{
		return track_mask_;
	}
	
	int get_track_coord()
	{
		return track_coord_;
	}
	
	int get_lock_time()
	{
		return lock_time_;
	}
	
	
	float get_kp(int channel)
	{
		return pid_[channel].get_kp();
	}
	
	float get_ki(int channel)
	{
		return pid_[channel].get_ki();
	}
	
	float get_kd(int channel)
	{
		return pid_[channel].get_kd();
	}
	
	float get_dead_zone(int channel)
	{
		return pid_[channel].get_dead_zone();
	}
	
	float get_max_limit(int channel)
	{
		return pid_[channel].get_max_limit();
	}
	
	
};
















#endif


















