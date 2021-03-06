#include "zscam_client.h"

#include <iostream>
#include <functional>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
# pragma warning(disable: 4786)
#endif

#include <locale.h>
#include <stdio.h>
#include <cassert>

#define SI_SUPPORT_IOSTREAMS
#if defined(SI_SUPPORT_IOSTREAMS) && !defined(_UNICODE)
# include <fstream>
#endif

//#define SI_CONVERT_GENERIC
//#define SI_CONVERT_ICU
//#define SI_CONVERT_WIN32
#include "SimpleIni.h"

#ifdef SI_CONVERT_ICU
// if converting using ICU then we need the ICU library
# pragma comment(lib, "icuuc.lib")
#endif

#ifdef _WIN32
# include <tchar.h>
#else // !_WIN32
# define TCHAR      char
# define _T(x)      x
# define _tprintf   printf
# define _tmain     main
#endif // _WIN32





zscam_client::zscam_client(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.plainTextEdit_all_boxes->setMaximumBlockCount(500);
	ui.plainTextEdit_min_boxes->setMaximumBlockCount(500);
	ui.plainTextEdit_ptz_calib->setMaximumBlockCount(500);
	
	
	width = 960;
	height = 540;
	going = 0;
	cx_ = width / 2;
	cy_ = height / 2;
	mouse_prex_ = width / 2;
	mouse_prey_ = height / 2;
	
	draw_poly_mask_mode_ = 0;
	show_cursor_mode_ = 0;
	show_detect_box_mask_ = STEREO_DEPTH_COORD_MASK;
	ptz_calib_mode_ = 0;
	
	show_mot_ = 1;
	show_sot_ = 0;

	
	show_poly_mask_mode_ = 0; 
	poly_mask_points_[0].clear();
	poly_mask_points_[1].clear();
	
	pixmap_ = QPixmap(width, height);
	
	save_avi_ = 0;
	save_pic_ = 0;
	record_count_ = 0;
	screenshot_count_ = 0;
	record_frame_count_ = 0;
	
	cam_open_ = 0;
	run_thread_ = NULL;
	
	ptz_open_ = 0;
	ptz_track_mode_ = 0;
	ptz_track_mask_ = PTZ_ALL_MASK;
	
	search_ = new search_camera("zynq_stereo_camera", BROAD_CAST_PORT, 5, 0);
	camera_ = new stereo_camera(0);
	boards_ = new search_camera("zscam_board", BROAD_BORAD_PORT, 5, 0);
	
	xfilter_ = new stereo_filter(camera_);
	xptz_ = new ptz_ctl_visca;
	xtrack_ = new ptz_track(xptz_, 150, 1);
	
	
	
	timer_slow_ = new QTimer(this);
	
	load_config("zscam_client.ini");

	connect(this, SIGNAL(fresh_frame_signal()), this, SLOT(do_fresh_frame()), Qt::QueuedConnection);
	connect(ui.label_video, SIGNAL(Mouse_Pressed(int, int)), this, SLOT(do_video_label_mouse_pressed(int, int)));
	
	connect(ui.toolButton_UL, SIGNAL(released()), this, SLOT(toolButton_pantilt_stop()));
	connect(ui.toolButton_U, SIGNAL(released()), this, SLOT(toolButton_pantilt_stop()));
	connect(ui.toolButton_UR, SIGNAL(released()), this, SLOT(toolButton_pantilt_stop()));
	connect(ui.toolButton_L, SIGNAL(released()), this, SLOT(toolButton_pantilt_stop()));
	connect(ui.toolButton_R, SIGNAL(released()), this, SLOT(toolButton_pantilt_stop()));
	connect(ui.toolButton_DL, SIGNAL(released()), this, SLOT(toolButton_pantilt_stop()));
	connect(ui.toolButton_D, SIGNAL(released()), this, SLOT(toolButton_pantilt_stop()));
	connect(ui.toolButton_DR, SIGNAL(released()), this, SLOT(toolButton_pantilt_stop()));
	
	connect(ui.toolButton_tele, SIGNAL(released()), this, SLOT(toolButton_zoom_stop()));
	connect(ui.toolButton_wide, SIGNAL(released()), this, SLOT(toolButton_zoom_stop()));
	
	connect(timer_slow_, SIGNAL(timeout()), this, SLOT(do_timer_slow_timeout()));
	timer_slow_->start(1000);
	
	search_->run();
	boards_->run();
}

zscam_client::~zscam_client()
{
	timer_slow_->stop();
	close_camera();
	xptz_->close_device();
	save_config("zscam_client.ini");
	
	
	delete timer_slow_;
	delete xtrack_; 
	delete xptz_;
	delete xfilter_;
	delete camera_;
	
	
}


 
void zscam_client::on_pushButton_open_camera_clicked() 
{
	int ret;
	string ip = ui.comboBox_ip->currentText().toStdString();
	int index = ui.comboBox_stream_number->currentIndex();

	if (!cam_open_)
	{
		ret = open_camera(ip, index);
		if (ret < 0) {
			QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("打开失败，请检查设备是否上电或IP填写是否正确"));			
			return;
		} 
		init_ui();
		ui.pushButton_open_camera->setText(QString::fromLocal8Bit("关闭"));
		
	}	
	else
	{
		close_camera();
		ui.pushButton_open_camera->setText(QString::fromLocal8Bit("连接"));
	}	
	
}

int zscam_client::open_camera(string ip, int index)
{
	int ret;
	
	ret = camera_->open_device(ip.c_str(), 7070, 9090, index);
	if (ret < 0) {
		 return -1;
	}
	cam_open_ = 1;
	run_thread_ = new std::thread([this] () {stream_process();});
	return 0;
}

int zscam_client::close_camera()
{
	cam_open_ = 0; 
	if (run_thread_)
	{
		run_thread_->join();
		delete run_thread_;
		run_thread_ = NULL;
	}
	camera_->close_device();
	return 0;
}

void zscam_client::stream_process()
{
	std::vector<unsigned char> frame_buffer;
	std::vector<struct stereo_detect_box> detect_boxes;
	struct stereo_gyro_angle gyro_angle;
	
	cout << "create client stream thread!\n";
	while(cam_open_)
	{
		if (!camera_->is_opened()) {
			std::this_thread::sleep_for (std::chrono::seconds(1));
			continue;
		}
		
		int ret = camera_->query_frame(5);
		if (ret < 0) {
			std::this_thread::sleep_for (std::chrono::seconds(1));
			continue;
		}
		
		frame_buffer.clear();
		detect_boxes.clear();
		memset((void *)&gyro_angle, 0, sizeof(struct stereo_gyro_angle));
		camera_->get_image(frame_buffer);
		camera_->get_detect_boxes(detect_boxes);
		camera_->get_gyro_angle(gyro_angle);
		
		xfilter_->compute(detect_boxes, number_state_, focus_box_, stable_state_);
		if ((detect_mode_) && (ptz_track_mode_))
		{
			xtrack_->set_focus_pose(focus_box_.pan, focus_box_.tilt, focus_box_.zoom, number_state_, stable_state_);
		}	
		
		{
			std::unique_lock<std::mutex> lock(mux_);
			detect_boxes_ = detect_boxes;
			gyro_angle_ = gyro_angle; 
			bool ok = pixmap_.loadFromData((const unsigned char *)&frame_buffer[0], frame_buffer.size());
			if (!ok)
				continue;
		}
		  
		if (save_avi_ == 1)
		{
			ret = mrecord.open_media(avi_name_.c_str(), pixmap_.width(), pixmap_.height(), 15);
			if (ret < 0) {
				save_avi_ = 2;
			} else {
				save_avi_ = 3;
			}
		}	
		else if (save_avi_ == 3) {
			ret = mrecord.write_frame((char *)&frame_buffer[0], frame_buffer.size(), 1);
			record_frame_count_++;
			if ((ret < 0) || (record_frame_count_ > 10000)) {
				mrecord.close_media();
				save_avi_ = 2; 
			}
		}
		
		if (save_pic_)
		{
			mrecord.save_picture(pic_name_.c_str(), (unsigned char *)&frame_buffer[0], frame_buffer.size());
			save_pic_ = 0;
		}
		 
		
		emit fresh_frame_signal();
	}	
	cout << "leave client stream thread!\n";
}

void zscam_client::do_fresh_frame()
{
	int ret;
	QPixmap pixmap;
	std::vector<struct stereo_detect_box> detect_boxes;
	struct stereo_gyro_angle gyro_angle;
	{
		std::unique_lock<std::mutex> lock(mux_);

		pixmap = pixmap_;
		detect_boxes = detect_boxes_;
		gyro_angle = gyro_angle_;
	}
	
	for (int i = 0; i < detect_boxes.size(); i++)
	{
		struct stereo_detect_box &detect_box = detect_boxes[i];
		string str = gen_detect_box_str(detect_box, show_detect_box_mask_);
		ui.plainTextEdit_all_boxes->appendPlainText(QString::fromStdString(str));
	}
		
	if (show_mot_)
		show_detect_boxes(&pixmap, detect_boxes);	
	
	if (number_state_ != STEREO_FILTER_NO_TARGET)
	{
		string str = gen_detect_box_str(focus_box_, show_detect_box_mask_);
		str += " ";
		str += to_string(number_state_);
		str += " ";
		str += to_string(stable_state_);
		ui.plainTextEdit_min_boxes->appendPlainText(QString::fromStdString(str));
		
		if (show_sot_) {
			Qt::PenStyle style = (stable_state_ == 1) ? Qt::SolidLine : Qt::DotLine;
			QColor color = (number_state_ == STEREO_FILTER_SINGLE_TARGET) ? Qt::blue : Qt::darkBlue ;
			show_detect_box(&pixmap, focus_box_, Qt::blue, style, 2);
		} 
	}	
	
	
	ui.doubleSpinBox_gyro_roll->setValue(gyro_angle.roll);
	ui.doubleSpinBox_gyro_pitch->setValue(gyro_angle.pitch);
	

	if (show_poly_mask_mode_) {
		show_poly_mask(&pixmap, poly_mask_points_[0], Qt::yellow);
	}
	
	if (draw_poly_mask_mode_) {
		show_poly_mask_points(&pixmap, poly_mask_points_[1], Qt::red);
	}
	
	if (ptz_calib_mode_) {
		show_ptz_samples(&pixmap, Qt::white);
	}
		

	if (show_cursor_mode_) {
		show_center_cursor(&pixmap, (int)cx_, (int)cy_);
	}	
		
	int w = ui.label_video->width();
	int h = ui.label_video->height();
	QPixmap scale_pixmap = pixmap.scaled(w, h);
	
	
	show_mouse_press_point(&scale_pixmap, mouse_prex_, mouse_prey_);
	
	
	
	ui.label_video->setPixmap(scale_pixmap);
}


void zscam_client::do_timer_slow_timeout()
{
	
	//服务IP
	{
		vector<string> device_nodes;
		search_->get_device_nodes(device_nodes);
		
		for (int i = 0; i < device_nodes.size(); i++)
		{
			QString ip = QString::fromStdString(device_nodes[i]);
			int has = 0;
			for (int j = 0; j < ui.comboBox_ip->count(); j++)
			{
				if (ip == ui.comboBox_ip->itemText(j))
				{
					has = 1;
					break;
				}
			}
			if (!has)
				ui.comboBox_ip->addItem(ip);
		}	
		
		for (int i = 0; i < ui.comboBox_ip->count(); i++)
		{
			int has = 0;
			for (int j = 0; j < device_nodes.size(); j++)
			{
				QString ip = QString::fromStdString(device_nodes[j]);
				if (ip == ui.comboBox_ip->itemText(i))
				{
					has = 1;
					break;
				}	
			}	
			if (!has)
			{
				ui.comboBox_ip->removeItem(i);
			}
		}	
	}
	
	
	//设备IP
	{
		vector<string> device_nodes;
		boards_->get_device_nodes(device_nodes);
		
		for (int i = 0; i < device_nodes.size(); i++)
		{
			QString ip = QString::fromStdString(device_nodes[i]);
			int has = 0;
			for (int j = 0; j < ui.comboBox_ip_update->count(); j++)
			{
				if (ip == ui.comboBox_ip_update->itemText(j))
				{
					has = 1;
					break;
				}
			}
			if (!has)
				ui.comboBox_ip_update->addItem(ip);
		}	
		
		for (int i = 0; i < ui.comboBox_ip_update->count(); i++)
		{
			int has = 0;
			for (int j = 0; j < device_nodes.size(); j++)
			{
				QString ip = QString::fromStdString(device_nodes[j]);
				if (ip == ui.comboBox_ip_update->itemText(i))
				{
					has = 1;
					break;
				}	
			}	
			if (!has)
			{
				ui.comboBox_ip_update->removeItem(i);
			}
		}

	}
	
	if (save_avi_ == 2)
	{
		save_avi_ = 0;
		ui.pushButton_record->setText(QString::fromLocal8Bit("录制"));
	}	
	
}
 
void zscam_client::do_video_label_mouse_pressed(int x, int y)
{
	int ret;
	cout << "video label: x= " << x << " y= " << y << endl;
	mouse_prex_ = x;
	mouse_prey_ = y;
	
	if (!cam_open_)
		return;
	
	float w = ui.label_video->width();
	float h = ui.label_video->height();
	
	int sx = (float)x / w * (float)width;
	int sy = (float)y / h * (float)height;
	cout << "stereo camera: x= " << sx << " y= " << sy << endl;
	
	if (draw_poly_mask_mode_)
	{
		poly_mask_points_[1].push_back(std::make_pair(sx, sy));
	}


	struct stereo_detect_box detect_box;
	ret = camera_->get_detect_point(sx, sy, detect_box);
	if ((ret < 0) || (detect_box.d <= 0))
		return;	

	string str = gen_detect_box_str(detect_box, show_detect_box_mask_);
	ui.plainTextEdit_all_boxes->appendPlainText(QString::fromStdString(str)); 	
 
	if (ptz_calib_mode_)
	{
		int pan_pose = 0, tilt_pose = 0, zoom_pose = 0;
		ret = xptz_->get_pantilt_position(&pan_pose, &tilt_pose);
		if (ret < 0)
		{
			printf("Failed to get pan tilt position!\n");
			return;
		}	
		ret = xptz_->get_zoom_position(&zoom_pose);
		if (ret < 0)
		{
			printf("Failed to get zoom position!\n");
			return;
		}	
		
		ptz_pose_.val[0] = pan_pose;
		ptz_pose_.val[1] = tilt_pose;
		ptz_pose_.val[2] = zoom_pose;
		detect_pose_ = detect_box;

		show_ptz_sample();
	}
	else
	{
		if (!detect_mode_)
		{
			if (ptz_track_mode_)
				xtrack_->set_focus_pose(detect_box.pan, detect_box.tilt, detect_box.zoom);
			else {
				xptz_->set_pantilt_absolute_position(detect_box.pan, detect_box.tilt, xptz_->get_max_pan_speed(), xptz_->get_max_tilt_speed());
				xptz_->set_zoom_absolute_position(detect_box.zoom, xptz_->get_max_zoom_speed());
					
			}
		}
	}	
}

 
void zscam_client::init_ui()
{
	int ret;
	int value;
	float fvalue;
	string svalue;
	
	if (!cam_open_)
		return;
	 
	ret = camera_->get_value("cx", fvalue);
	if (ret == 0) {
		cx_ = fvalue;
	}	
	
	ret = camera_->get_value("cy", fvalue);
	if (ret == 0) {
		cy_ = fvalue;
	}
	
	ret = camera_->get_value("http_out_channel", value);
	if (ret == 0) {
		if (value == 0) {
			ui.radioButton_channel0_mode->setChecked(true);
			ui.radioButton_channel1_mode->setChecked(false);
		} else {
			ui.radioButton_channel0_mode->setChecked(false);
			ui.radioButton_channel1_mode->setChecked(true);
		}	
	}
	 
	ret = camera_->get_value("match_mode", value); 
	if (ret == 0) {

		if (value == STEREO_CAMERA_MATCH_OPEN_MODE) {
			match_mode_ = STEREO_CAMERA_MATCH_OPEN_MODE;
			ui.checkBox_match_mode->setCheckState(Qt::Checked);
		} else {
			match_mode_ = STEREO_CAMERA_MATCH_CLOSE_MODE;
			ui.checkBox_match_mode->setCheckState(Qt::Unchecked); 
		}	 
	}
	
	ret = camera_->get_value("match_edge_th", value);
	if (ret == 0) {
		ui.spinBox_match_edge_th->setValue(value);
	}
	
	ret = camera_->get_value("match_P1", value);
	if (ret == 0) {
		ui.spinBox_match_P1->setValue(value);
	}
	
	ret = camera_->get_value("match_P2", value);
	if (ret == 0) {
		ui.spinBox_match_P2->setValue(value);
	}
		
	ret = camera_->get_value("match_check_th", value);
	if (ret == 0) {
		ui.spinBox_match_check_th->setValue(value);
	}
	 
	ret = camera_->get_value("bg_mode", value);
	if (ret == 0) {

		if (value == STEREO_CAMERA_BG_OPEN_MODE) {
			bg_mode_ = STEREO_CAMERA_BG_OPEN_MODE;
			ui.checkBox_bg_mode->setCheckState(Qt::Checked);
		} else {
			bg_mode_ = STEREO_CAMERA_BG_CLOSE_MODE;
			ui.checkBox_bg_mode->setCheckState(Qt::Unchecked); 
		}	 
	}
	 
	ret = camera_->get_value("bg_color_dist", value);
	if (ret == 0) {
		ui.spinBox_bg_color_dist->setValue(value);
	}	
		
	ret = camera_->get_value("bg_color_match", value);
	if (ret == 0) {
		ui.spinBox_bg_color_match->setValue(value);
	}

	ret = camera_->get_value("bg_space_dist", value);
	if (ret == 0) {
		ui.spinBox_bg_space_dist->setValue(value);
	}	
		
	ret = camera_->get_value("bg_space_match", value);
	if (ret == 0) {
		ui.spinBox_bg_space_match->setValue(value);
	}

	ret = camera_->get_value("bg_disp_dist", value);
	if (ret == 0) {
		ui.spinBox_bg_disp_dist->setValue(value);
	}	
		
	ret = camera_->get_value("bg_disp_match", value);
	if (ret == 0) {
		ui.spinBox_bg_disp_match->setValue(value);
	}
	
	ret = camera_->get_value("bg_update_radio", value);
	if (ret == 0) {
		ui.spinBox_bg_update_radio->setValue(value);
	}	
		
	ret = camera_->get_value("median_mode", value);
	if (ret == 0) { 
		if (value == STEREO_CAMERA_MEDIAN_OPEN_MODE) {
			median_mode_ = STEREO_CAMERA_MEDIAN_OPEN_MODE;
			ui.checkBox_median_mode->setCheckState(Qt::Checked);
		} else {
			median_mode_ = STEREO_CAMERA_MEDIAN_CLOSE_MODE;
			ui.checkBox_median_mode->setCheckState(Qt::Unchecked); 
		}	 
	}
	
	ret = camera_->get_value("tex_mode", value);
	if (ret == 0) { 
		if (value == STEREO_CAMERA_TEX_OPEN_MODE) {
			tex_mode_ = STEREO_CAMERA_TEX_OPEN_MODE;
			ui.checkBox_tex_mode->setCheckState(Qt::Checked);
		} else {
			tex_mode_ = STEREO_CAMERA_TEX_CLOSE_MODE;
			ui.checkBox_tex_mode->setCheckState(Qt::Unchecked); 
		}	 
	}
	
	ret = camera_->get_value("space_mode", value);
	if (ret == 0) { 
		if (value == STEREO_CAMERA_SPACE_OPEN_MODE) {
			space_mode_ = STEREO_CAMERA_SPACE_OPEN_MODE;
			ui.checkBox_space_mode->setCheckState(Qt::Checked);
		} else {
			space_mode_ = STEREO_CAMERA_SPACE_CLOSE_MODE;
			ui.checkBox_space_mode->setCheckState(Qt::Unchecked); 
		}	 
	}
	
	ret = camera_->get_value("morph_mode", value);
	if (ret == 0) { 
		if (value == STEREO_CAMERA_MORPH_OPEN_MODE) {
			morph_mode_ = STEREO_CAMERA_MORPH_OPEN_MODE;
			ui.checkBox_morph_mode->setCheckState(Qt::Checked);
		} else {
			morph_mode_ = STEREO_CAMERA_MORPH_CLOSE_MODE;
			ui.checkBox_morph_mode->setCheckState(Qt::Unchecked); 
		}	 
	}
	
	ret = camera_->get_value("install_height", value);
	if (ret == 0) {
		ui.spinBox_install_height->setValue(value);
	}
	
	ret = camera_->get_value("install_x_angle", fvalue);
	if (ret == 0) {
		ui.doubleSpinBox_install_x_angle->setValue(fvalue);
	}
	
	ret = camera_->get_value("install_z_angle", fvalue);
	if (ret == 0) {
		ui.doubleSpinBox_install_z_angle->setValue(fvalue);
	}
	
	ret = camera_->get_value("detect_minx", value);
	if (ret == 0) {
		ui.spinBox_detect_minx->setValue(value);
	}
	
	ret = camera_->get_value("detect_maxx", value);
	if (ret == 0) {
		ui.spinBox_detect_maxx->setValue(value);
	}
	
	ret = camera_->get_value("detect_miny", value);
	if (ret == 0) {
		ui.spinBox_detect_miny->setValue(value);
	}
	
	ret = camera_->get_value("detect_maxy", value);
	if (ret == 0) {
		ui.spinBox_detect_maxy->setValue(value);
	}
	
	ret = camera_->get_value("detect_minz", value);
	if (ret == 0) {
		ui.spinBox_detect_minz->setValue(value);
	}
	
	ret = camera_->get_value("detect_maxz", value);
	if (ret == 0) {
		ui.spinBox_detect_maxz->setValue(value);
	}
	
	ret = camera_->get_value("post_tex_th", value);
	if (ret == 0) {
		ui.spinBox_post_tex_th->setValue(value);
	}
	
	ret = camera_->get_value("post_tex_sum_th", value);
	if (ret == 0) {
		ui.spinBox_post_tex_sum_th->setValue(value);
	}
	
	ret = camera_->get_value("poly_mode", value);
	if (ret == 0) { 
		if (value == STEREO_CAMERA_POLY_OPEN_MODE) {
			poly_mode_ = STEREO_CAMERA_POLY_OPEN_MODE;
			ui.checkBox_poly_mode->setCheckState(Qt::Checked);
		} else {
			poly_mode_ = STEREO_CAMERA_POLY_CLOSE_MODE;
			ui.checkBox_poly_mode->setCheckState(Qt::Unchecked); 
		}	 
	}
	
	vector<pair<float, float> > poly_mask_points;
	ret = camera_->get_poly_mask(poly_mask_points);
	if (ret == 0)
	{
		poly_mask_points_[0] = poly_mask_points;
	}	
	
	ret = camera_->get_value("post_gray_mode", value);
	if (ret == 0) { 
		if (value == STEREO_CAMERA_POST_GRAY_OPEN_MODE) {
			post_gray_mode_ = STEREO_CAMERA_POST_GRAY_OPEN_MODE;
			ui.checkBox_post_gray_mode->setCheckState(Qt::Checked);
		} else {
			post_gray_mode_ = STEREO_CAMERA_POST_GRAY_CLOSE_MODE;
			ui.checkBox_post_gray_mode->setCheckState(Qt::Unchecked); 
		}	 
	}
	
	
	ret = camera_->get_value("detect_mode", value);
	if (ret == 0) {
		detect_mode_ = value;
		if (detect_mode_ == STEREO_CAMERA_DETECT_CLOSE_MODE) {
			ui.pushButton_detect_mode->setText(QString::fromLocal8Bit("开始检测"));
		} else {
			ui.pushButton_detect_mode->setText(QString::fromLocal8Bit("停止检测"));
		}
	}

	ret = camera_->get_value("detect_min_wsize", value);
	if (ret == 0) {
		ui.spinBox_detect_min_wsize->setValue(value);
	}

	ret = camera_->get_value("detect_min_space_size", value);
	if (ret == 0) {
		ui.spinBox_detect_min_space_size->setValue(value);
	}
	
	ret = camera_->get_value("detect_min_iou", fvalue);
	if (ret == 0) {
		ui.doubleSpinBox_min_iou->setValue((double)fvalue);
	}
 
	ret = camera_->get_value("track_max_cost", fvalue);
	if (ret == 0) {
		ui.doubleSpinBox_track_max_cost->setValue(fvalue);
	}
	
	ret = camera_->get_value("detect_min_nms_dist", value);
	if (ret == 0) {
		ui.spinBox_detect_min_nms_dist->setValue(value);
	}

	ret = camera_->get_value("track_mode", value);
	if (ret == 0) {
		track_mode_ = value;
		if (value) {
			ui.checkBox_track_mode->setCheckState(Qt::Checked); 
		} else {
			ui.checkBox_track_mode->setCheckState(Qt::Unchecked);  
		}	
	}
	
	ret = camera_->get_value("http_out_mode", value);
	if (ret == 0) {
		ui.comboBox_http_out_mode->setCurrentIndex(value);
	}	

	ret = camera_->get_value("http_out_quality", value);
	if (ret == 0) {
		ui.spinBox_stream_quality->setValue(value);
	}
	
	ret = camera_->get_value("board", svalue);
	if (ret == 0) {
		ui.lineEdit_board->setText(QString::fromStdString(svalue));
	}
	
	ret = camera_->get_value("version", svalue);
	if (ret == 0) {
		ui.lineEdit_version->setText(QString::fromStdString(svalue));
	}	
	 
	ret = camera_->get_value("serial_number", svalue);
	if (ret == 0) {
		ui.lineEdit_serial_number->setText(QString::fromStdString(svalue));
	}
 
	ret = camera_->get_value("ip", svalue);
	if (ret == 0) {
		ui.lineEdit_ip->setText(QString::fromStdString(svalue));
	}
	
	ret = camera_->get_value("netmask", svalue);
	if (ret == 0) {
		ui.lineEdit_netmask->setText(QString::fromStdString(svalue));
	}
	
	ret = camera_->get_value("gateway", svalue);
	if (ret == 0) {
		ui.lineEdit_gateway->setText(QString::fromStdString(svalue));
	}
	
	ret = camera_->get_value("mac", svalue);
	if (ret == 0) {
		ui.lineEdit_mac->setText(QString::fromStdString(svalue));
	}
	
	ret = camera_->get_value("udp_ip", svalue);
	if (ret == 0) {
		ui.lineEdit_udp_ip->setText(QString::fromStdString(svalue));
	}
	
	ret = camera_->get_value("udp_port", value);
	if (ret == 0) {
		ui.lineEdit_udp_port->setText(QString::fromStdString(std::to_string(value)));
	}
	
}



void zscam_client::show_center_cursor(QPixmap *dst, int x, int y)
{
	if ((x >= 0) && (y >= 0))
	{
		QPainter painter(dst);
		painter.setPen(QPen(Qt::yellow, 1.5, Qt::SolidLine));
		painter.drawLine(0, y, width - 1, y);
		painter.drawLine(x, 0, x, height - 1);
		int w = 20, h = 20;
		painter.drawRect(x - w / 2, y - h / 2, w, h);
	}	
}


void zscam_client::show_mouse_press_point(QPixmap *dst, int x, int y)
{
	int len = 5;
	if ((x >= 0) && (y >= 0))
	{
		QPainter painter(dst);
		painter.setPen(QPen(Qt::darkBlue, 2, Qt::SolidLine));
		painter.drawRoundRect(x - len / 2, y - len / 2, len, len);
	}
}

void zscam_client::show_detect_boxes(QPixmap *dst, vector<struct stereo_detect_box> &detect_boxes)
{
	for (int i = 0; i < detect_boxes.size(); i++)
	{
		struct stereo_detect_box &detect_box = detect_boxes[i];
		int id = detect_box.id;
		QColor color = QColor(255, 255, 255);
		if (id > 0)  {
			color = QColor((id * 10) % 255, (id * 20) % 255, (id * 30) % 255);
		}	
		show_detect_box(dst, detect_box, color);	
	}
}

void zscam_client::show_detect_box(QPixmap *dst, struct stereo_detect_box &detect_box, QColor color, Qt::PenStyle style, float line)
{
	QPainter painter(dst);
	
	int box_x = detect_box.box_x;
	int box_y = detect_box.box_y;
	int box_w = detect_box.box_w;
	int box_h = detect_box.box_h;
	painter.setPen(QPen(color, line, style));
	painter.drawRect(box_x, box_y, box_w, box_h);
	
	painter.setPen(QPen(color, 4, Qt::SolidLine));
	painter.drawPoint(detect_box.x, detect_box.y);
	
	
	painter.setPen(QPen(color, 2, Qt::SolidLine));
	string str = gen_detect_box_str(detect_box, show_detect_box_mask_);
	painter.drawText(box_x, box_y, QString::fromStdString(str));
	
}


string zscam_client::gen_detect_box_str(struct stereo_detect_box &detect_box, int mask)
{
	std::stringstream os;
	os << "id=" << detect_box.id << " ";
	if (mask & STEREO_GRAPH_COORD_MASK) {
		os << "box=(" << detect_box.box_x << " " << detect_box.box_y << " " << detect_box.box_w << " " << detect_box.box_h << ") ";	
	}
	
	if (mask & STEREO_DEPTH_COORD_MASK) {
		os << "x=" << detect_box.x << " y=" << detect_box.y << " d=" << detect_box.d << " ";
	}
		
	
	if (mask & STEREO_CAMERA_COORD_MASK) {
		os << "xcm=" << detect_box.xcm << " ycm=" << detect_box.ycm << " zcm=" << detect_box.zcm << " ";
	}
	
	if (mask & STEREO_ROOM_COORD_MASK) {
		os << "xtcm=" << detect_box.xtcm << " ytcm=" << detect_box.ytcm << " ztcm=" << detect_box.ztcm << " ";
	}
	
	if (mask & STEREO_BALL_COORD_MASK) {
		os << "xa=" << detect_box.xa << " ya=" << -detect_box.ya << " r=" << detect_box.r << " ";
	}
	
	if (mask & STEREO_PTZ_COORD_MASK) {
		os << "pan=" << detect_box.pan << " tilt=" << detect_box.tilt << " zoom=" << detect_box.zoom << " ";
	}
	return os.str();
}

void zscam_client::show_poly_mask(QPixmap *dst, vector<pair<float, float> > &points, QColor color)
{
	if (points.size() < 4)
		return;
	
	QPainter painter(dst);
	for (int i = 0; i < points.size() - 1; i++)
	{
		painter.setPen(QPen(color, 2, Qt::SolidLine));
		painter.drawLine(points[i].first, points[i].second, points[i + 1].first, points[i + 1].second);
	}
	painter.drawLine(points[points.size() - 1].first, points[points.size() - 1].second, points[0].first, points[0].second);
	
}

void zscam_client::show_poly_mask_points(QPixmap *dst, vector<pair<float, float> > &points, QColor color)
{
	int len = 11;
	QPainter painter(dst);
	for (int i = 0; i < points.size(); i++)
	{
		painter.setPen(QPen(color, 1.5, Qt::SolidLine));
		int x = points[i].first;
		int y = points[i].second;
		painter.drawLine(x - len / 2, y, x + len / 2, y);
		painter.drawLine(x, y - len / 2, x, y + len / 2);
	}

}


void zscam_client::on_pushButton_update_clicked()
{
	int ret;
	QString fileName = QFileDialog::getOpenFileName(0, QString::fromLocal8Bit("选择文件"), QCoreApplication::applicationDirPath(), QString::fromLocal8Bit("所有文件(*)"));
	
	QTextCodec *code = QTextCodec::codecForName("GB2312");//解决中文路径问题
    std::string src_name = code->fromUnicode(fileName).data();
	
	
	string ip = ui.comboBox_ip_update->currentText().toStdString();
	string ftp_host = ip;
	ftp_host += string(":21");
	ftp_client xftp(ftp_host.c_str(), "anonymous", "null");
 
	ret = xftp.connect_device();
	if (ret < 0) {
		QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("连接双目失败"));
		return;
	}
		 
	ftp_dialog dialog(&xftp, src_name.c_str(), "/opt/zscam/config/update.zip");
	
	
	dialog.exec();
	
	int isOk = dialog.get_success();
	if (!isOk) {
		QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("上传升级包失败"));
		return;
	}
	QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("升级成功，请重启设备"));

}


void zscam_client::on_pushButton_open_ptz_clicked()
{
	int ret;
	string ptz_name = ui.comboBox_ptz_name->currentText().toStdString();

	if (!ptz_open_)
	{
		ret = xptz_->open_device(ptz_name.c_str());
		if (ret < 0) {
			QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("打开失败"));			
			return;
		}
		
		ptz_open_ = 1;
	//	init_ui();
		ui.pushButton_open_ptz->setText(QString::fromLocal8Bit("关闭"));
		
	}	
	else
	{
		ptz_open_ = 0;
		xptz_->close_device();
		ui.pushButton_open_ptz->setText(QString::fromLocal8Bit("连接"));
	}
}

 
void zscam_client::on_comboBox_ptz_track_mode_currentIndexChanged(int index)
{
	ptz_track_mode_ = index;
	xtrack_->set_track_mode(index);
		
	if (ui.checkBox_ptz_track_pan_mask->checkState() == Qt::Checked)
		ptz_track_mask_ |= PTZ_PAN_MASK;
	else
		ptz_track_mask_ &= ~PTZ_PAN_MASK;	
	if (ui.checkBox_ptz_track_tilt_mask->checkState() == Qt::Checked)
		ptz_track_mask_ |= PTZ_TILT_MASK;
	else
		ptz_track_mask_ &= ~PTZ_TILT_MASK;
	if (ui.checkBox_ptz_track_zoom_mask->checkState() == Qt::Checked)
		ptz_track_mask_ |= PTZ_ZOOM_MASK;
	else
		ptz_track_mask_ &= ~PTZ_ZOOM_MASK;
	xtrack_->set_track_mask(ptz_track_mask_);
}


void zscam_client::on_checkBox_ptz_calib_en_stateChanged(int arg1)
{
	switch (arg1)
	{
		case Qt::Unchecked:
		{
			ptz_calib_mode_ = 0;
			ui.pushButton_set_ptz_samples->setEnabled(false);
		}break;
		
		
		case Qt::Checked:
		{
			ptz_calib_mode_ = 1;
			ui.pushButton_set_ptz_samples->setEnabled(true);
		}break;
		
		
	}
}


void zscam_client::on_pushButton_clear_ptz_samples_clicked()
{
	ptz_samples_.clear();
	ui.spinBox_ptz_sample_size->setValue(0);
	ui.plainTextEdit_ptz_calib->clear();
}

void zscam_client::on_pushButton_add_ptz_sample_clicked()
{
	stringstream os;
	os << ptz_samples_.size() << ": (" << ptz_pose_.val[0] << ", " << ptz_pose_.val[1] << ", "  << ptz_pose_.val[2] << ") (";
	os << detect_pose_.x << ", " << detect_pose_.y << ", "  << (int)detect_pose_.r << ")";
	
	ptz_samples_.push_back(make_pair(ptz_pose_, detect_pose_));
	ui.spinBox_ptz_sample_size->setValue(ptz_samples_.size());
	
	string str = os.str();
	ui.plainTextEdit_ptz_calib->appendPlainText(QString::fromStdString(str));
}
 
void zscam_client::on_pushButton_set_ptz_samples_clicked()
{
	int ret; 
	int ptz_install_mode = ui.comboBox_ptz_install_mode->currentIndex();
	ret = camera_->set_value("ptz_install_mode", ptz_install_mode);
	if (ret < 0) {
		QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("云台校正失败"));			
		return;
	}
	
	ret = camera_->set_ptz_samples(ptz_samples_);
	if (ret < 0) {
		QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("云台校正失败"));			
		return;
	}
}

void zscam_client::show_pid_para()
{
	int channel = ui.comboBox_pid_channel->currentIndex();
	
	ui.doubleSpinBox_pid_kp->setValue(xtrack_->get_kp(channel));
	ui.doubleSpinBox_pid_ki->setValue(xtrack_->get_ki(channel));
	ui.doubleSpinBox_pid_kd->setValue(xtrack_->get_kd(channel));
	ui.doubleSpinBox_pid_dead_zone->setValue(xtrack_->get_dead_zone(channel));
	ui.doubleSpinBox_pid_max_limit->setValue(xtrack_->get_max_limit(channel));
}


void zscam_client::show_ptz_sample()
{ 
	ui.spinBox_ptz_pose0->setValue(ptz_pose_.val[0]);
	ui.spinBox_ptz_pose1->setValue(ptz_pose_.val[1]);
	ui.spinBox_ptz_pose2->setValue(ptz_pose_.val[2]);
	 
	ui.spinBox_detect_pose0->setValue(detect_pose_.x);
	ui.spinBox_detect_pose1->setValue(detect_pose_.y);
	ui.spinBox_detect_pose2->setValue((int)detect_pose_.r);
}

void zscam_client::show_ptz_samples(QPixmap *dst, QColor color)
{
	int x, y;
	int len = 30;
	QPainter painter(dst); 
	painter.setPen(QPen(color, 1.5, Qt::SolidLine));
	for (int i = 0; i < ptz_samples_.size(); i++) 
	{
		x = ptz_samples_[i].second.x;
		y = ptz_samples_[i].second.y; 
		
		painter.drawLine(x - len / 2, y, x + len / 2, y);
		painter.drawLine(x, y - len / 2, x, y + len / 2);
	}	
	
	x = detect_pose_.x;
	y = detect_pose_.y;
	painter.drawLine(x - len / 2, y, x + len / 2, y);
	painter.drawLine(x, y - len / 2, x, y + len / 2);
}


int zscam_client::load_config(const char *config_name)
{
	int ret; 
	int value;
	float fvalue;
	std::string svalue;
	
	FILE *fp = fopen(config_name, "a+");
	if (fp == NULL)
		return -1;
	fclose(fp);
	
	CSimpleIniA ini;
	ini.SetUnicode();
	ret = ini.LoadFile(config_name);
	if (ret < 0) {
		printf("Failed to load %s\n", config_name);
		return -1;
	}
		
	
	svalue = ini.GetValue("ptz_track", "ptz_name", "");
	ptz_name_ = svalue;
	ui.comboBox_ptz_name->clear();
	ui.comboBox_ptz_name->addItem(QString::fromStdString(ptz_name_));
 
	svalue = ini.GetValue("ptz_track", "pids", "");
	ret = xtrack_->pid_paras_from_string(svalue);
	if (ret == 0) {
		show_pid_para();
	}
	
	svalue = ini.GetValue("ptz_track", "track_mode", "0");
	value = atoi(svalue.c_str());
	xtrack_->set_track_mode(value);
	ui.comboBox_ptz_track_mode->setCurrentIndex(value);

	svalue = ini.GetValue("ptz_track", "track_mask", "0");
	value = atoi(svalue.c_str());
	xtrack_->set_track_mask(value);
	if (value & PTZ_PAN_MASK) {
		ui.checkBox_ptz_track_pan_mask->setCheckState(Qt::Checked);
	} else {
		ui.checkBox_ptz_track_pan_mask->setCheckState(Qt::Unchecked);
	}
	
	if (value & PTZ_TILT_MASK) {
		ui.checkBox_ptz_track_tilt_mask->setCheckState(Qt::Checked);
	} else {
		ui.checkBox_ptz_track_tilt_mask->setCheckState(Qt::Unchecked);
	}
	
	if (value & PTZ_ZOOM_MASK) {
		ui.checkBox_ptz_track_zoom_mask->setCheckState(Qt::Checked);
	} else {
		ui.checkBox_ptz_track_zoom_mask->setCheckState(Qt::Unchecked);
	}
	
	svalue = ini.GetValue("ptz_track", "min_number_count", "2");
	value = atoi(svalue.c_str()); 
	xfilter_->set_min_number_count(value);
	ui.spinBox_min_number_count->setValue(value);
	
	svalue = ini.GetValue("ptz_track", "max_number_count", "4");
	value = atoi(svalue.c_str());
	xfilter_->set_max_number_count(value);
	ui.spinBox_max_number_count->setValue(value);
	
	svalue = ini.GetValue("ptz_track", "stable_distance", "50");
	fvalue = atof(svalue.c_str());
	xfilter_->set_stable_distance(fvalue);
	ui.doubleSpinBox_stable_distance->setValue(fvalue);
	
	svalue = ini.GetValue("ptz_track", "min_stable_count", "16");
	value = atoi(svalue.c_str());
	xfilter_->set_min_stable_count(value);
	ui.spinBox_min_stable_count->setValue(value);
	 
	ret = ini.SaveFile(config_name);
	if (ret < 0) {
		printf("Failed to save %s\n", config_name);
		return -1;
	}
	
	
	return 0;
}

int zscam_client::save_config(const char *config_name)
{
	int ret;
	string svalue;
	CSimpleIniA ini;
	ini.SetUnicode();
	ret = ini.LoadFile(config_name);
	if (ret < 0) {
		printf("Failed to load %s\n", config_name);
		return -1;
	}
	
	string ptz_name = ui.comboBox_ptz_name->currentText().toStdString();
	ini.SetValue("ptz_track", "ptz_name", ptz_name.c_str());
	 
	ret = xtrack_->pid_paras_to_string(svalue);
	if (ret == 0) {
		ini.SetValue("ptz_track", "pids", svalue.c_str());
	}
	
	ini.SetValue("ptz_track", "track_mode", to_string(xtrack_->get_track_mode()).c_str());
	ini.SetValue("ptz_track", "track_mask", to_string(xtrack_->get_track_mask()).c_str());
	
	ini.SetValue("ptz_track", "min_number_count", to_string(xfilter_->get_min_number_count()).c_str());
	ini.SetValue("ptz_track", "max_number_count", to_string(xfilter_->get_max_number_count()).c_str());
	ini.SetValue("ptz_track", "stable_distance", to_string(xfilter_->get_stable_distance()).c_str());
	ini.SetValue("ptz_track", "min_stable_count", to_string(xfilter_->get_min_stable_count()).c_str());
	
	 
	ret = ini.SaveFile(config_name);
	if (ret < 0) {
		printf("Failed to save %s\n", config_name);
		return -1;
	}
	
	return 0;
}

































