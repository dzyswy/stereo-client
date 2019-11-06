#include "ptz_track.h"
#include "json/json.h"


  
  
  
using namespace std;



#ifdef _WIN32
int gettimeofday(struct timeval *tp, void *tzp)
{
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	tm.tm_year = wtm.wYear - 1900;
	tm.tm_mon = wtm.wMonth - 1;
	tm.tm_mday = wtm.wDay;
	tm.tm_hour = wtm.wHour;
	tm.tm_min = wtm.wMinute;
	tm.tm_sec = wtm.wSecond;
	tm. tm_isdst = -1;
	clock = mktime(&tm);
	tp->tv_sec = clock;
	tp->tv_usec = wtm.wMilliseconds * 1000;
	return (0);
}
#endif

ptz_track::ptz_track(ptz_ctl_visca *ptz, float period, int debug)
{
	ptz_ = ptz; 
	period_ = period;
	debug_ = debug;
	
	position_range_[PTZ_PAN_CHANNEL][0] = ptz_->get_min_pan_position();
	position_range_[PTZ_PAN_CHANNEL][1] = ptz_->get_max_pan_position();
	position_range_[PTZ_TILT_CHANNEL][0] = ptz_->get_min_tilt_position();
	position_range_[PTZ_TILT_CHANNEL][1] = ptz_->get_max_tilt_position();
	position_range_[PTZ_ZOOM_CHANNEL][0] = ptz_->get_min_zoom_position();
	position_range_[PTZ_ZOOM_CHANNEL][1] = ptz_->get_max_zoom_position();
	 
	speed_range_[PTZ_PAN_CHANNEL][0] = ptz_->get_min_pan_speed();
	speed_range_[PTZ_PAN_CHANNEL][1] = ptz_->get_max_pan_speed();
	speed_range_[PTZ_TILT_CHANNEL][0] = ptz_->get_min_tilt_speed();
	speed_range_[PTZ_TILT_CHANNEL][1] = ptz_->get_max_tilt_speed();
	speed_range_[PTZ_ZOOM_CHANNEL][0] = ptz_->get_min_zoom_speed();
	speed_range_[PTZ_ZOOM_CHANNEL][1] = ptz_->get_max_zoom_speed();
	
	
	
	track_mode_ = PTZ_TRACK_STOP_TRACK_MODE;
	track_mask_ = PTZ_ALL_MASK; 
	trig_state_ = PTZ_TRACK_TARGET_SEARCH; 
	
	memset(&focus_pose_, 0, sizeof(focus_pose_));
	
	for (int i = 0; i < PTZ_MAX_CHANNEL; i++)
	{
		xpid_[i] = new pid_inc(debug);
	}	
	
	run();
}

ptz_track::~ptz_track()
{
	stop();	
	for (int i = 0; i < PTZ_MAX_CHANNEL; i++)
	{
		delete xpid_[i];
	}	
}


void ptz_track::run()
{
	going = 1;
	run_thread_ = new std::thread([this] () {track_process();}); 
}

void ptz_track::stop()
{
	going = 0;
	if (run_thread_)
	{
		run_thread_->join();
		delete run_thread_;
		run_thread_ = NULL;
	}
	ptz_->set_pantilt_stop();
	ptz_->set_zoom_stop(); 
}

void ptz_track::set_focus_pose(int pan_pose, int tilt_pose, int zoom_pose, int number_state, int stable_state)
{
	struct stereo_ptz_pose focus_pose;
	focus_pose.val[PTZ_PAN_CHANNEL] = pan_pose;
	focus_pose.val[PTZ_TILT_CHANNEL] = tilt_pose;
	focus_pose.val[PTZ_ZOOM_CHANNEL] = zoom_pose;
	for (int i = 0; i < PTZ_MAX_CHANNEL; i++)
	{
		int &val = focus_pose.val[i];
		val = (val < position_range_[i][0]) ? position_range_[i][0] : ((val > position_range_[i][1]) ? position_range_[i][1] : val);
	}	
	 
	std::unique_lock<std::mutex> lock(mux_);
	focus_pose_ = focus_pose;
	number_state_ = number_state;
	stable_state_ = stable_state;

}



void ptz_track::track_process()
{
	int ret; 
	int pre_number_state = STEREO_FILTER_NO_TARGET;
	while(going)
	{ 
		if ((!track_mode_) || (!ptz_->is_opened()))
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			continue;
		}	
		
		struct timeval tv[2];
		gettimeofday(&tv[0], NULL);
		
		struct stereo_ptz_pose focus_pose;
		int number_state, stable_state;
		{
			std::unique_lock<std::mutex> lock(mux_);
			focus_pose = focus_pose_;
			number_state = number_state_;
			stable_state = stable_state_;
		}
		
		struct stereo_ptz_pose current_pose; 
		ret = ptz_->get_pantilt_position(&current_pose.val[PTZ_PAN_CHANNEL], &current_pose.val[PTZ_TILT_CHANNEL]);
		if (ret < 0) {
			printf("Failed to get pan tilt position!\n");
			std::this_thread::sleep_for(std::chrono::seconds(1));
			continue;
		}
		ret = ptz_->get_zoom_position(&current_pose.val[PTZ_ZOOM_CHANNEL]);
		if (ret < 0) {
			printf("Failed to get zoom position!\n");
			std::this_thread::sleep_for(std::chrono::seconds(1));
			continue;
		} 
		
		if (debug_)
		{
			printf("timestamp: %ld-%ld\n", tv[0].tv_sec, tv[0].tv_usec);
			printf("focus pose: %d %d %d\n", focus_pose.val[0], focus_pose.val[1], focus_pose.val[2]);
			printf("current pose: %d %d %d\n", current_pose.val[0], current_pose.val[1], current_pose.val[2]);	 
		}
		 
		/*
		 * 1.连续跟踪模式
		 * 2.触发跟踪模式
		*/ 
		if (track_mode_ == PTZ_TRACK_CONTINUOUS_TRACK_MODE)
		{
			struct ptz_track_ptz_speed ptz_speed = {0};
			compute(focus_pose, current_pose, ptz_speed);
			pantilt_move_speed(ptz_speed.val[PTZ_PAN_CHANNEL], ptz_speed.val[PTZ_TILT_CHANNEL]);
			zoom_move_speed(ptz_speed.val[PTZ_ZOOM_CHANNEL]); 
			
		}	
		else if (track_mode_ == PTZ_TRACK_LOCK_TRACK_MODE)
		{
			compute(focus_pose, current_pose, number_state, pre_number_state, stable_state);
		}
		pre_number_state = number_state;
			 
		gettimeofday(&tv[1], NULL);
		 
		int time_used = (tv[1].tv_sec - tv[0].tv_sec) * 1000 + (tv[1].tv_usec - tv[0].tv_usec) / 1000;
		int time_leave = period_ - time_used;
		if (time_leave > 1)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(time_leave));
		}	 
	}	
}

void ptz_track::compute(struct stereo_ptz_pose &focus_pose, struct stereo_ptz_pose &current_pose, struct ptz_track_ptz_speed &ptz_speed)
{	
	float err[PTZ_MAX_CHANNEL] = {0};
	for (int i = 0; i < PTZ_MAX_CHANNEL; i++)
	{
		err[i] = (focus_pose.val[i] - current_pose.val[i]) / (position_range_[i][1] - position_range_[i][0]);
	}	
	
	struct ptz_track_ptz_speed pid_speed;
	for (int i = 0; i < PTZ_MAX_CHANNEL; i++)
	{
		pid_speed.val[i] = round(xpid_[i]->compute(err[i]) * 10 * speed_range_[i][1]);
	}
	
	if (!(track_mask_ & PTZ_PAN_MASK))
		pid_speed.val[PTZ_PAN_CHANNEL] = 0;
	
	if (!(track_mask_ & PTZ_TILT_MASK))
		pid_speed.val[PTZ_TILT_CHANNEL] = 0;
	
	if (!(track_mask_ & PTZ_ZOOM_MASK))
		pid_speed.val[PTZ_ZOOM_CHANNEL] = 0;
	
	ptz_speed = pid_speed;
	if (debug_) {
		printf("ptz speed: %d %d %d\n", ptz_speed.val[0], ptz_speed.val[1], ptz_speed.val[2]);
	}
	
}

void ptz_track::compute(struct stereo_ptz_pose &focus_pose, struct stereo_ptz_pose &current_pose, int number_state, int pre_number_state, int stable_state)
{
	switch(trig_state_)
	{
		case PTZ_TRACK_TARGET_SEARCH:
		{  
			if ((pre_number_state == STEREO_FILTER_NO_TARGET) && (number_state != STEREO_FILTER_NO_TARGET))
			{
				trig_state_ = PTZ_TRACK_TARGET_APPEAR;
			}	
		 
		}break;
		
		case PTZ_TRACK_TARGET_APPEAR:
		{
			if (number_state != STEREO_FILTER_NO_TARGET)
			{
				if (stable_state)
				{
					int pan_position = focus_pose.val[PTZ_PAN_CHANNEL];
					int tilt_position = focus_pose.val[PTZ_TILT_CHANNEL];
					int zoom_position = focus_pose.val[PTZ_ZOOM_CHANNEL];
					 
					if (!(track_mask_ & PTZ_PAN_MASK))
						pan_position = current_pose.val[PTZ_PAN_CHANNEL];
					
					if (!(track_mask_ & PTZ_TILT_MASK))
						tilt_position = current_pose.val[PTZ_TILT_CHANNEL];
					
					if (!(track_mask_ & PTZ_ZOOM_MASK))
						zoom_position = current_pose.val[PTZ_ZOOM_CHANNEL];
					
					ptz_->set_pantilt_absolute_position(pan_position, tilt_position, ptz_->get_max_pan_speed(), ptz_->get_max_tilt_speed());
					ptz_->set_zoom_absolute_position(zoom_position, ptz_->get_max_zoom_speed());
					
					trig_state_ = PTZ_TRACK_TARGET_STABLE; 
				}	
				
			}
			else
			{
				trig_state_ = PTZ_TRACK_TARGET_SEARCH;
			}	
		}break;
		
		case PTZ_TRACK_TARGET_STABLE:
		{
			if (number_state == STEREO_FILTER_NO_TARGET)
			{
				trig_state_ = PTZ_TRACK_TARGET_SEARCH;
			}	
		}break;
		default:
			break;
		
	}
}


void ptz_track::pantilt_move_speed(int panSpeed, int tiltSpeed)
{
	int pan_speed = abs(panSpeed);
	int tilt_speed = abs(tiltSpeed);
	if ((panSpeed == 0) && (tiltSpeed == 0)) 
	{
		ptz_->set_pantilt_stop();
	}	
	else 
	{
		if ((panSpeed != 0) && (tiltSpeed != 0)) 
		{
			if ((panSpeed > 0) && (tiltSpeed > 0))
			{
				ptz_->set_pantilt_upright(pan_speed, tilt_speed);
			}	
			else if ((panSpeed > 0) && (tiltSpeed < 0))
			{
				ptz_->set_pantilt_downright(pan_speed, tilt_speed);
			}
			else if ((panSpeed < 0) && (tiltSpeed > 0))
			{
				ptz_->set_pantilt_upleft(pan_speed, tilt_speed);
			}	
			else if ((panSpeed < 0) && (tiltSpeed < 0))
			{
				ptz_->set_pantilt_downleft(pan_speed, tilt_speed);
			}	
		}
		else if ((panSpeed != 0) && (tiltSpeed == 0))
		{
			if (panSpeed < 0) {
				ptz_->set_pantilt_left(pan_speed, tilt_speed);
			}
			else {
				ptz_->set_pantilt_right(pan_speed, tilt_speed);
			}
		}	
		else if ((panSpeed == 0) && (tiltSpeed != 0))
		{
			if (tiltSpeed < 0) {
				ptz_->set_pantilt_down(pan_speed, tilt_speed);
			}
			else {
				ptz_->set_pantilt_up(pan_speed, tilt_speed);
			}
		}	
	}
}

void ptz_track::zoom_move_speed(int zoomSpeed)
{
	int zoom_speed = abs(zoomSpeed);
	if (zoomSpeed == 0)
	{
		ptz_->set_zoom_stop();
	}	
	else
	{
		if (zoomSpeed > 0)
		{
			ptz_->set_zoom_tele(zoom_speed - 1);
		}	
		else
		{
			ptz_->set_zoom_wide(zoom_speed - 1);
		}		
	}	
}


int ptz_track::pid_paras_to_string(std::string &value)
{
	try {
		Json::Value jroot;
		Json::Value jpids;
		
		
		for (int i = 0; i < PTZ_MAX_CHANNEL; i++)
		{ 
			Json::Value jpid;
			
			jpid["kp"] = get_kp(i);
			jpid["ki"] = get_ki(i);
			jpid["kd"] = get_kd(i);
			jpid["dead_zone"] = get_dead_zone(i);
			jpid["max_limit"] = get_max_limit(i);
			
			jpids.append(jpid);
		}	
	
		jroot["pids"] = jpids;
		
	//	value = jroot.toStyledString();
		Json::StreamWriterBuilder builder;
		builder["indentation"] = "";
		value = Json::writeString(builder, jroot);
	}
	catch(std::exception &ex)
    {
        printf( "jsoncpp struct error: %s.\n", ex.what());
        return -1;
	}
	return 0;
}

int ptz_track::pid_paras_from_string(std::string value)
{
	try {
		Json::Reader reader;
		Json::Value jroot;
		Json::Value jpids;
		 
		if (!reader.parse(value, jroot))
			return -1;
		
		if (jroot["pids"].empty())
			return -1; 
		
		jpids = jroot["pids"];
		for (int i = 0; i < jpids.size(); i++)
		{  
			Json::Value jpid;
			jpid = jpids[i];
			
			set_kp(i, jpid["kp"].asFloat());
			set_ki(i, jpid["ki"].asFloat());
			set_kd(i, jpid["kd"].asFloat());
			set_dead_zone(i, jpid["dead_zone"].asFloat());
			set_max_limit(i, jpid["max_limit"].asFloat());
		} 
		
	} catch(std::exception &ex)
    {
        printf( "jsoncpp struct error: %s.\n", ex.what());
        return -1;
	}
	return 0;
}






