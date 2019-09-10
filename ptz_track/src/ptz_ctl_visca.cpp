#include "ptz_ctl_visca.h"
#include <libvisca/libvisca.h>



ptz_ctl_visca::ptz_ctl_visca()
{
	pan_speed_ = 8;
	tilt_speed_ = 6;
	zoom_speed_ = 1;
	
	min_pan_speed_ = 1;
	max_pan_speed_ = 24;
	min_tilt_speed_ = 1;
	max_tilt_speed_ = 20;
	min_zoom_speed_ = 0;
	max_zoom_speed_ = 7;
	
	interface_ = new VISCACamera_t;
	camera = new VISCACamera_t;
}

ptz_ctl_visca::~ptz_ctl_visca()
{
	delete camera;
	delete interface_;
	
}



int ptz_ctl_visca::open_device(const char *dev_name) 
{
	int camera_num;
    if (VISCA_open_serial(interface_, dev_name)!=VISCA_SUCCESS) {
        fprintf(stderr,"camera_ui: unable to open serial device %s\n",dev_name);
        return -1;
    }

    interface_->broadcast=0;
    VISCA_set_address(interface_, camera_num);
    camera.address=1;
    VISCA_clear(interface_, camera);
    VISCA_get_camera_info(interface_, camera);
	
	const unsigned char VISCA_POWER_ON = 2;
	const unsigned char VISCA_POWER_OFF = 3;
	VISCA_set_power(interface_, camera, VISCA_POWER_ON);
	
	return 0;
} 

int ptz_ctl_visca::close_device() 
{
	VISCA_close_serial(interface_);
	return 0;
} 
	
	

	
int ptz_ctl_visca::set_pantilt_left(int pan_speed) 
{
	int panSpeed = gen_valid_pan_speed(pan_speed);
	int tiltSpeed = tilt_speed_;
	
	if (VISCA_set_pantilt_left(interface_, camera, panSpeed, tiltSpeed) != VISCA_SUCCESS)
		return -1;
	return 0;
} 

int ptz_ctl_visca::set_pantilt_right(int pan_speed) 
{
	int panSpeed = gen_valid_pan_speed(pan_speed);
	int tiltSpeed = tilt_speed_;
	
	if (VISCA_set_pantilt_right(interface_, camera, panSpeed, tiltSpeed) != VISCA_SUCCESS)
		return -1;
	return 0;
} 

int ptz_ctl_visca::set_pantilt_up(int tilt_speed) 
{
	int panSpeed = pan_speed_;
	int tiltSpeed = gen_valid_tilt_speed(tilt_speed);
	
	if (VISCA_set_pantilt_up(interface_, camera, panSpeed, tiltSpeed) != VISCA_SUCCESS)
		return -1;
	return 0;
} 

int ptz_ctl_visca::set_pantilt_down(int tilt_speed) 
{
	int panSpeed = pan_speed_;
	int tiltSpeed = gen_valid_tilt_speed(tilt_speed);
	
	if (VISCA_set_pantilt_down(interface_, camera, panSpeed, tiltSpeed) != VISCA_SUCCESS)
		return -1;
	return 0;
} 

int ptz_ctl_visca::set_pantilt_upleft(int pan_speed, int tilt_speed) 
{
	int panSpeed = gen_valid_pan_speed(pan_speed);
	int tiltSpeed = gen_valid_tilt_speed(tilt_speed);
	
	if (VISCA_set_pantilt_upleft(interface_, camera, panSpeed, tiltSpeed) != VISCA_SUCCESS)
		return -1;
	return 0;
} 

int ptz_ctl_visca::set_pantilt_upright(int pan_speed, int tilt_speed) 
{
	int panSpeed = gen_valid_pan_speed(pan_speed);
	int tiltSpeed = gen_valid_tilt_speed(tilt_speed);
	
	if (VISCA_set_pantilt_upright(interface_, camera, panSpeed, tiltSpeed) != VISCA_SUCCESS)
		return -1;
	return 0;
} 

int ptz_ctl_visca::set_pantilt_downleft(int pan_speed, int tilt_speed) 
{
	int panSpeed = gen_valid_pan_speed(pan_speed);
	int tiltSpeed = gen_valid_tilt_speed(tilt_speed);
	
	if (VISCA_set_pantilt_downleft(interface_, camera, panSpeed, tiltSpeed) != VISCA_SUCCESS)
		return -1;
	return 0;
}
 
int ptz_ctl_visca::set_pantilt_downright(int pan_speed, int tilt_speed) 
{
	int panSpeed = gen_valid_pan_speed(pan_speed);
	int tiltSpeed = gen_valid_tilt_speed(tilt_speed);
	
	if (VISCA_set_pantilt_downright(interface_, camera, panSpeed, tiltSpeed) != VISCA_SUCCESS)
		return -1;
	return 0;
} 

int ptz_ctl_visca::set_pantilt_stop() 
{
	int panSpeed = pan_speed_;
	int tiltSpeed = tilt_speed_;
	
	if (VISCA_set_pantilt_stop(interface_, camera, panSpeed, tiltSpeed) != VISCA_SUCCESS)
		return -1;
	return 0;
} 
	
int ptz_ctl_visca::set_pantilt_absolute_position(int pan_position, int tilt_position, int pan_speed, int tilt_speed) 
{
	int panSpeed = gen_valid_pan_speed(pan_speed);
	int tiltSpeed = gen_valid_tilt_speed(tilt_speed);
	if (VISCA_set_pantilt_absolute_position(&interface_, &camera, panSpeed, tiltSpeed, pan_position, tilt_position) != VISCA_SUCCESS)
		return -1;
	return 0;
} 

int ptz_ctl_visca::set_pantilt_relative_position(int pan_position, int tilt_position, int pan_speed, int tilt_speed) 
{
	int panSpeed = gen_valid_pan_speed(pan_speed);
	int tiltSpeed = gen_valid_tilt_speed(tilt_speed);
	if (VISCA_set_pantilt_relative_position(&interface_, &camera, panSpeed, tiltSpeed, pan_position, tilt_position) != VISCA_SUCCESS)
		return -1;
	return 0;
} 

int ptz_ctl_visca::set_pantilt_home() 
{
	if (VISCA_set_pantilt_home(&interface_, &camera) != VISCA_SUCCESS)
		return -1;
	return 0;
} 
	
int ptz_ctl_visca::set_zoom_tele(int zoom_speed) 
{
	int zoomSpeed = gen_valid_zoom_speed(zoom_speed);
	if (VISCA_set_zoom_tele_speed(&interface_, &camera, zoomSpeed) != VISCA_SUCCESS)
		return -1;
	return 0;
} 

int ptz_ctl_visca::set_zoom_wide(int zoom_speed) 
{
	int zoomSpeed = gen_valid_zoom_speed(zoom_speed);
	if (VISCA_set_zoom_wide_speed(&interface_, &camera, zoomSpeed) != VISCA_SUCCESS)
		return -1;
	return 0;
} 

int ptz_ctl_visca::set_zoom_stop() 
{
	if (VISCA_set_zoom_stop(&interface_, &camera) != VISCA_SUCCESS)
		return -1;
	return 0;
} 


int ptz_ctl_visca::set_zoom_absolute_position(int zoom_position, int zoom_speed) 
{
	if (VISCA_set_zoom_value(&interface_, &camera, zoom_position) != VISCA_SUCCESS)
		return -1;
	return 0;
} 
	
	
int ptz_ctl_visca::get_pantilt_position(int *pan_position, int *tilt_position) 
{
	short panPosition = 0, tiltPosition = 0;
	if (VISCA_get_pantilt_position(&interface_, &camera, (int16_t *)&panPosition, (int16_t *)&tiltPosition) != VISCA_SUCCESS)
		return -1;
	*pan_position = panPosition;
	*tilt_position = tiltPosition;
	return 0;
} 

int ptz_ctl_visca::get_zoom_position(int *zoom_position) 
{
	uint16_t zoomPosition;
	if (VISCA_get_zoom_value(&interface_, &camera, (uint16_t *)zoomPosition) != VISCA_SUCCESS)
		return -1;
	*zoom_position = zoomPosition;
	return 0;
} 






