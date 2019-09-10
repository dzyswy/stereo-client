#include "ptz_track.h"





ptz_track::ptz_track(ptz_ctl_visca *ptz, fit_calib *calib, float period)
{
	ptz_ = ptz;
	calib_ = calib;
	period_ = period;
	
	track_mode_ = 0;
	track_mask_ = PTZ_TRACK_PTZ_ALL_MASK;
	track_coord_ = FIT_CALIB_BALL_COORD;
	lock_time_ = 10;
	lock_state_ = PTZ_TRACK_TRACK_UNLOCKED;
	
	
	run_thread_ = NULL;
}

ptz_track::~ptz_track()
{
	stop();	
}


int ptz_track::run()
{
	going = 1;
	run_thread_ = new std::thread([this] () {track_process();});
	return 0;
}

int ptz_track::stop()
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
	return 0;
}

int ptz_track::set_detect_box(struct stereo_detect_box &value)
{
	int ret;
	
	struct ptz_track_focus_pixel focus_pixel;
	focus_pixel.val[FIT_CALIB_GRAPH_COORD][FIT_CALIB_PTZ_PAN] = value.x;
	focus_pixel.val[FIT_CALIB_GRAPH_COORD][FIT_CALIB_PTZ_TILT] = value.y;
	focus_pixel.val[FIT_CALIB_GRAPH_COORD][FIT_CALIB_PTZ_ZOOM] = value.d;
	
	focus_pixel.val[FIT_CALIB_CAMEAR_COORD][FIT_CALIB_PTZ_PAN] = value.xcm;
	focus_pixel.val[FIT_CALIB_CAMEAR_COORD][FIT_CALIB_PTZ_TILT] = value.ycm;
	focus_pixel.val[FIT_CALIB_CAMEAR_COORD][FIT_CALIB_PTZ_ZOOM] = value.zcm;
	
	focus_pixel.val[FIT_CALIB_ROOM_COORD][FIT_CALIB_PTZ_PAN] = value.xtcm;
	focus_pixel.val[FIT_CALIB_ROOM_COORD][FIT_CALIB_PTZ_TILT] = value.ytcm;
	focus_pixel.val[FIT_CALIB_ROOM_COORD][FIT_CALIB_PTZ_ZOOM] = value.ztcm;
	
	focus_pixel.val[FIT_CALIB_BALL_COORD][FIT_CALIB_PTZ_PAN] = value.xa;
	focus_pixel.val[FIT_CALIB_BALL_COORD][FIT_CALIB_PTZ_TILT] = value.ya;
	focus_pixel.val[FIT_CALIB_BALL_COORD][FIT_CALIB_PTZ_ZOOM] = value.r;
	
	for (int i = 0; i < FIT_CALIB_PTZ_MAX_CHANNEL; i++)
	{
		ret = calib_->pixel_to_pose(focus_pixel.val[track_coord_][i], focus_pose_.val[i], track_coord_, i);
		if (ret < 0)
			return -1;
	}
	
	return 0;
}

void ptz_track::track_process()
{
	int ret;
	int track_mode_ = 0;
	while(going)
	{
		struct timeval tv[2];
		gettimeofday(&tv[0], NULL);
		
		
		int ptz_pose[FIT_CALIB_PTZ_MAX_CHANNEL]; 
		ret = ptz_->get_pantilt_position(&ptz_pose[PTZ_TRACK_PTZ_PAN], &ptz_pose[PTZ_TRACK_PTZ_TILT]);
		if (ret < 0) {
			printf("Failed to get pan tilt position!\n");
			std::this_thread::sleep_for(std::chrono::seconds(3));
			continue;
		}
		ret = ptz_->get_zoom_position(&ptz_pose[PTZ_TRACK_PTZ_ZOOM]);
		if (ret < 0) {
			printf("Failed to get zoom position!\n");
			std::this_thread::sleep_for(std::chrono::seconds(3));
			continue;
		}
		
		float ptz_diff[FIT_CALIB_PTZ_MAX_CHANNEL];
		for (int i = 0; i < FIT_CALIB_PTZ_MAX_CHANNEL; i++)
		{
			ptz_diff[i] = focus_pose_.val[i] - (float)ptz_pose[i];
		}	
		
		bool pan_mask = (track_mask_ & PTZ_TRACK_PTZ_PAN_MASK) ? true : false;
		bool tilt_mask = (track_mask_ & PTZ_TRACK_PTZ_TILT_MASK) ? true : false;
		bool zoom_mask = (track_mask_ & PTZ_TRACK_PTZ_ZOOM_MASK) ? true : false;
		bool pan_move = (fabs(ptz_diff[PTZ_TRACK_PTZ_PAN]) > get_dead_zone(PTZ_TRACK_PTZ_PAN)) ? true : false;
		bool tilt_move = (fabs(ptz_diff[PTZ_TRACK_PTZ_TILT]) > get_dead_zone(PTZ_TRACK_PTZ_TILT)) ? true : false;
		bool zoom_move = (fabs(ptz_diff[PTZ_TRACK_PTZ_ZOOM]) > get_dead_zone(PTZ_TRACK_PTZ_ZOOM)) ? true : false;
		bool move_pan = (pan_mask && pan_move) ? true : false;
		bool move_tilt = (tilt_mask && tilt_move) ? true : false;
		bool move_zoom = (zoom_mask && zoom_move) ? true : false;

		
		 
		/*
		 * 1.连续跟踪模式
		 * 2.跟踪锁定模式
		*/ 
		if (track_mode_ == PTZ_TRACK_CONTINUOUS_TRACK_MODE)
		{
			float speed[FIT_CALIB_PTZ_MAX_CHANNEL];
			for (int i = 0; i < FIT_CALIB_PTZ_MAX_CHANNEL; i++)
			{
				speed[i] = pid_[i].compute(ptz_diff[i]);
			}	
			
			int panSpeed = pan_mask ? std::round(speed[FIT_CALIB_PTZ_PAN]) : 0;
			int tiltSpeed = tilt_mask ? std::round(speed[FIT_CALIB_PTZ_TILT]) : 0;
			int zoomSpeed = zoom_mask ? std::round(speed[FIT_CALIB_PTZ_ZOOM]) : 0;
			
			int pan_speed = abs(panSpeed);
			int tilt_speed = abs(tiltSpeed);
			int zoom_speed = abs(zoomSpeed);
			
			if ((panSpeed == 0) && (tiltSpeed == 0)) 
			{
				ptz_->set_pantilt_stop(pan_speed, tilt_speed);
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
			
			if (zoomSpeed == 0)
			{
				ptz_->set_zoom_stop();
			}	
			else
			{
				if (zoomSpeed > 0)
				{
					ptz_->set_zoom_tele_speed(zoom_speed - 1);
				}	
				else
				{
					ptz_->set_zoom_wide_speed(zoom_speed - 1);
				}		
			}	
			
		}	
		else if (track_mode_ == PTZ_TRACK_LOCK_TRACK_MODE)
		{
			switch(lock_state_)
			{
				case PTZ_TRACK_TRACK_UNLOCKED:
				{ 
					if (position_statble_count) 
					{
						if (move_pan || move_tilt)
						{
							int pan_position = move_pan ? focus_pose_.val[PTZ_TRACK_PTZ_PAN] : ptz_pose[PTZ_TRACK_PTZ_PAN];
							int tilt_position = move_tilt ? focus_pose_.val[PTZ_TRACK_PTZ_TILT] : ptz_pose[PTZ_TRACK_PTZ_TILT];
							ptz_->set_pantilt_absolute_position(&pan_position, &tilt_position, ptz_->get_max_pan_speed(), ptz_->get_max_tilt_speed());
						}	
						
						if (move_zoom)
						{
							int zoom_position = focus_pose_.val[PTZ_TRACK_PTZ_ZOOM];
							ptz_->set_zoom_absolute_position(&zoom_position, ptz_->get_max_zoom_speed());
						}	
						
						lock_state_ = PTZ_TRACK_TRACK_LOCKED;
						lock_timeout_ = time(0) + lock_time_;
					}
					
					
						
				}break;
				case PTZ_TRACK_TRACK_LOCKED:
				{
					if (time(0) > lock_timeout_)
					{
						lock_state_ = PTZ_TRACK_TRACK_UNLOCKED;
					}	
				}break;
				default:
					break;
				
			}
		}
			 
		gettimeofday(&tv[1], NULL);
		
		float time_used = (tv[1].tv_sec - tv[0].tv_sec) * 1000 + (tv[1].tv_usec - tv[0].tv_usec) / 1000;
		float time_leave = period_ - time_used;
		if (time_leave > 1)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(time_leave));
		}	 
	}	
}








