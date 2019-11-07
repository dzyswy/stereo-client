#ifndef ZSCAM_CLIENT_H
#define ZSCAM_CLIENT_H


#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <ctime>
#include <QtWidgets/QWidget>
#include <QStyleFactory>
#include <QMessageBox>
#include <QImage>
#include <QTimer>
#include <QFile>
#include <QPixmap>
#include <QPainter>
#include <QPen>
#include <QFileDialog>
#include <QDir>
#include <QTextCodec>
//#include <opencv2/opencv.hpp>


#include "ui_zscam_client.h"
#include "search_camera.h"
#include "stereo_camera.h"
#include "media_record.h"
#include "stereo_filter.h"
#include "ptz_ctl_visca.h"
#include "ptz_track.h"
#include "ftp_client.h"
#include "ftp_dialog.h"






using namespace std; 





 



enum ZSCAM_MOUSE_MODE_TYPE
{
	MOUSE_MODE_DEFAULT = 0,
	MOUSE_MODE_POLY_MASK = 1,
};


 


class zscam_client : public QWidget
{
	Q_OBJECT

public:
	zscam_client(QWidget *parent = 0);
	~zscam_client();
	
	
	
private:
	int open_camera(string ip, int index);
	int close_camera();
	void stream_process();
	void show_detect_boxes(QPixmap *dst, vector<struct stereo_detect_box> &target_boxes);
	void show_detect_box(QPixmap *dst, struct stereo_detect_box &detect_box, QColor color, Qt::PenStyle style = Qt::SolidLine, float line = 2);
	string gen_detect_box_str(struct stereo_detect_box &detect_box, int mask);
	void init_ui();
	void show_center_cursor(QPixmap *dst, int x, int y);
	void show_mouse_press_point(QPixmap *dst, int x, int y);
	void show_poly_mask(QPixmap *dst, vector<pair<float, float> > &points, QColor color);
	void show_poly_mask_points(QPixmap *dst, vector<pair<float, float> > &points, QColor color);
	void show_pid_para();
	void show_ptz_sample();
	void show_ptz_samples(QPixmap *dst, QColor color);
	int load_config(const char *config_name);
	int save_config(const char *config_name);
	
public:
	Ui::zscam_clientClass ui;
	int width;
	int height;
	int going;
	
	QTimer *timer_slow_; 
	
	search_camera *search_;
	stereo_camera *camera_;
	int cam_open_;

	QPixmap pixmap_;
	std::vector<struct stereo_detect_box> detect_boxes_;
	struct stereo_gyro_angle gyro_angle_;
	std::mutex mux_;
	std::condition_variable cond_;
	std::thread *run_thread_;
	
	float cx_;
	float cy_;
	int match_mode_;
	int bg_mode_;
	int median_mode_;
	int tex_mode_;
	int space_mode_;
	int morph_mode_;
	int poly_mode_;
	int post_gray_mode_;
	int detect_mode_;
	int track_mode_;
	
	
	int mouse_prex_;
	int mouse_prey_;
	
 
	media_record mrecord;
	int save_avi_;
	int save_pic_;
	std::string avi_name_;
	std::string pic_name_;
	int record_count_;
	int screenshot_count_;
	int record_frame_count_;

	int show_cursor_mode_;
	int show_detect_box_mask_;
	int show_mot_;
	int show_sot_;
	
	int draw_poly_mask_mode_;
	int show_poly_mask_mode_;
	vector<pair<float, float> > poly_mask_points_[2];
	
	
	stereo_filter *xfilter_;
	int number_state_;
	struct stereo_detect_box focus_box_; 
	int stable_state_;
	 
	
	ptz_ctl_visca *xptz_;
	ptz_track *xtrack_;
	
	int ptz_open_;
	int ptz_track_mode_;
	int ptz_track_mask_;
	std::string ptz_name_;
	
	int ptz_calib_mode_;
	
	struct stereo_ptz_pose ptz_pose_;
	struct stereo_detect_box detect_pose_;
	std::vector<std::pair<struct stereo_ptz_pose, struct stereo_detect_box> > ptz_samples_;

signals:
	void fresh_frame_signal();	
	
private slots:
	void on_pushButton_open_camera_clicked();
	void do_fresh_frame();
	void do_timer_slow_timeout();
	void do_video_label_mouse_pressed(int x, int y);
	
	
	void on_checkBox_match_mode_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				match_mode_ = STEREO_CAMERA_MATCH_CLOSE_MODE;
				break;
			case Qt::Checked:
				match_mode_ = STEREO_CAMERA_MATCH_OPEN_MODE;
				break;
		}
		camera_->set_value("match_mode", match_mode_);
	}

	 
	void on_spinBox_match_edge_th_valueChanged(const QString &arg1)
	{
		camera_->set_value("match_edge_th", arg1.toInt());
	}
		
	void on_spinBox_match_P1_valueChanged(const QString &arg1)
	{
		camera_->set_value("match_P1", arg1.toInt());
	}
		
	void on_spinBox_match_P2_valueChanged(const QString &arg1)
	{
		camera_->set_value("match_P2", arg1.toInt());
	}
		
	void on_spinBox_match_check_th_valueChanged(const QString &arg1)
	{
		camera_->set_value("match_check_th", arg1.toInt());
	}
	
	void on_checkBox_bg_mode_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				bg_mode_ = STEREO_CAMERA_BG_CLOSE_MODE;
				break;
			case Qt::Checked:
				bg_mode_ = STEREO_CAMERA_BG_OPEN_MODE;
				break;
		}
		camera_->set_value("bg_mode", bg_mode_);
	}
		 
	void on_spinBox_bg_color_dist_valueChanged(const QString &arg1)
	{
		camera_->set_value("bg_color_dist", arg1.toInt());
	}
		
	void on_spinBox_bg_color_match_valueChanged(const QString &arg1)
	{
		camera_->set_value("bg_color_match", arg1.toInt());
	}
		
	void on_spinBox_bg_space_dist_valueChanged(const QString &arg1)
	{
		camera_->set_value("bg_space_dist", arg1.toInt());
	}
		
	void on_spinBox_bg_space_match_valueChanged(const QString &arg1)
	{
		camera_->set_value("bg_space_match", arg1.toInt());
	}
		
	void on_spinBox_bg_disp_dist_valueChanged(const QString &arg1)
	{
		camera_->set_value("bg_disp_dist", arg1.toInt());
	}
		
	void on_spinBox_bg_disp_match_valueChanged(const QString &arg1)
	{
		camera_->set_value("bg_disp_match", arg1.toInt());
	}
		
	void on_spinBox_bg_update_radio_valueChanged(const QString &arg1)
	{
		camera_->set_value("bg_update_radio", arg1.toInt());
	}
		
		
	void on_pushButton_bg_init_clicked()
	{
		camera_->do_action("bg_init");
	}
		
	void on_checkBox_median_mode_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				median_mode_ = STEREO_CAMERA_MEDIAN_CLOSE_MODE;
				break;
			case Qt::Checked:
				median_mode_ = STEREO_CAMERA_MEDIAN_OPEN_MODE;
				break;
		}
		camera_->set_value("median_mode", median_mode_);
	}
	
	void on_checkBox_tex_mode_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				tex_mode_ = STEREO_CAMERA_TEX_CLOSE_MODE;
				break;
			case Qt::Checked:
				tex_mode_ = STEREO_CAMERA_TEX_OPEN_MODE;
				break;
		}
		camera_->set_value("tex_mode", tex_mode_);
	}
	
	void on_checkBox_space_mode_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				space_mode_ = STEREO_CAMERA_SPACE_CLOSE_MODE;
				break;
			case Qt::Checked:
				space_mode_ = STEREO_CAMERA_SPACE_OPEN_MODE;
				break;
		}
		camera_->set_value("space_mode", space_mode_);
	}
	
	void on_checkBox_morph_mode_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				morph_mode_ = STEREO_CAMERA_MORPH_CLOSE_MODE;
				break;
			case Qt::Checked:
				morph_mode_ = STEREO_CAMERA_MORPH_OPEN_MODE;
				break;
		}
		camera_->set_value("morph_mode", morph_mode_);
	}
 
	void on_spinBox_post_tex_th_valueChanged(const QString &arg1)
	{
		camera_->set_value("post_tex_th", arg1.toInt());
	}
		
	void on_spinBox_post_tex_sum_th_valueChanged(const QString &arg1)
	{
		camera_->set_value("post_tex_sum_th", arg1.toInt());
	}
	

	void on_spinBox_install_height_valueChanged(const QString &arg1)
	{
		camera_->set_value("install_height", arg1.toInt());
	}
		
	void on_doubleSpinBox_install_x_angle_valueChanged(const QString &arg1)
	{
		camera_->set_value("install_x_angle", (float)arg1.toDouble());
	}
		
	void on_doubleSpinBox_install_z_angle_valueChanged(const QString &arg1)
	{
		camera_->set_value("install_z_angle", (float)arg1.toDouble());
	}
		
	void on_spinBox_detect_minx_valueChanged(const QString &arg1)
	{
		camera_->set_value("detect_minx", arg1.toInt());
	}
		
	void on_spinBox_detect_maxx_valueChanged(const QString &arg1)
	{
		camera_->set_value("detect_maxx", arg1.toInt());
	}
		
	void on_spinBox_detect_miny_valueChanged(const QString &arg1)
	{
		camera_->set_value("detect_miny", arg1.toInt());
	}
		
	void on_spinBox_detect_maxy_valueChanged(const QString &arg1)
	{
		camera_->set_value("detect_maxy", arg1.toInt());
	}
		
	void on_spinBox_detect_minz_valueChanged(const QString &arg1)
	{
		camera_->set_value("detect_minz", arg1.toInt());
	}
		
	void on_spinBox_detect_maxz_valueChanged(const QString &arg1)
	{
		camera_->set_value("detect_maxz", arg1.toInt());
	}
		
	void on_checkBox_poly_mode_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				poly_mode_ = STEREO_CAMERA_POLY_CLOSE_MODE;
				break;
			case Qt::Checked:
				poly_mode_ = STEREO_CAMERA_POLY_OPEN_MODE;
				break;
		}
		camera_->set_value("poly_mode", poly_mode_);
	}
   	
	void on_checkBox_draw_poly_mask_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
			{
				draw_poly_mask_mode_ = 0;
				poly_mask_points_[1].clear();
				ui.pushButton_set_poly_mask->setEnabled(false);
			}break;
			case Qt::Checked:
			{
				draw_poly_mask_mode_ = 1;
				poly_mask_points_[1].clear();
				ui.pushButton_set_poly_mask->setEnabled(true);
			}break;
			default:
				break;
		}
	}
	
	void on_pushButton_clear_poly_mask_clicked()
	{
		poly_mask_points_[1].clear();
	}
	
	void on_pushButton_set_poly_mask_clicked()
	{ 
		camera_->set_poly_mask(poly_mask_points_[1]);
		camera_->get_poly_mask(poly_mask_points_[0]);
	}
	
	void on_checkBox_show_poly_mask_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				show_poly_mask_mode_ = 0;
				break;
			case Qt::Checked:
				show_poly_mask_mode_ = 1;
				break;
		}
		camera_->get_poly_mask(poly_mask_points_[0]);
	}
	
	void on_checkBox_post_gray_mode_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				post_gray_mode_ = 0;
				break;
			case Qt::Checked:
				post_gray_mode_ = 1;
				break;
		}
		camera_->set_value("post_gray_mode", post_gray_mode_);
	}

	void on_pushButton_detect_mode_clicked()
	{ 
		if (detect_mode_ == 0)
		{
			detect_mode_ = 1;
			ui.pushButton_detect_mode->setText(QString::fromLocal8Bit("停止检测"));
		}
		else
		{
			detect_mode_ = 0;
			ui.pushButton_detect_mode->setText(QString::fromLocal8Bit("开始检测"));
		}
		camera_->set_value("detect_mode", detect_mode_);
		camera_->set_value("track_mode", track_mode_);	  
	}

	void on_spinBox_detect_min_wsize_valueChanged(const QString &arg1)
	{
		camera_->set_value("detect_min_wsize", arg1.toInt());
	}

	void on_spinBox_detect_min_space_size_valueChanged(const QString &arg1)
	{
		camera_->set_value("detect_min_space_size", arg1.toInt());
	}

	void on_spinBox_detect_min_nms_dist_valueChanged(const QString &arg1)
	{
		camera_->set_value("detect_min_nms_dist", arg1.toInt());
	}
	
	void on_doubleSpinBox_min_iou_valueChanged(const QString &arg1)
	{
		camera_->set_value("detect_min_iou", (float)arg1.toDouble());
	}

	void on_checkBox_track_mode_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				track_mode_ = 0;
				break;
			case Qt::Checked:
				track_mode_ = 1;
				break;
		}
	}

	void on_doubleSpinBox_track_max_cost_valueChanged(const QString &arg1)
	{
		camera_->set_value("track_max_cost", (float)arg1.toDouble());
	}
	
	void on_comboBox_http_out_mode_currentIndexChanged(int index)
	{
		camera_->set_value("http_out_mode", index);
	}
	
	void on_radioButton_channel0_mode_clicked()
	{
		camera_->set_value("http_out_channel", 0);
	}

	void on_radioButton_channel1_mode_clicked()
	{
		camera_->set_value("http_out_channel", 1);
	}
	
	void on_spinBox_stream_quality_valueChanged(const QString &arg1)
	{
		camera_->set_value("http_out_quality", arg1.toInt());
	}
	
	void on_pushButton_save_config_clicked()
	{
		camera_->do_action("save_config");
	}
	
	void on_pushButton_reboot_clicked()
	{
		camera_->do_action("reboot", 1);
	}
	
	void on_checkBox_dhcp_stateChanged(int state)
	{
		if (state == Qt::Checked)
		{
			ui.lineEdit_ip->setEnabled(false);
			ui.lineEdit_netmask->setEnabled(false);
			ui.lineEdit_gateway->setEnabled(false);	
		}	
		else if (state == Qt::Unchecked)
		{
			ui.lineEdit_ip->setEnabled(true);
			ui.lineEdit_netmask->setEnabled(true);
			ui.lineEdit_gateway->setEnabled(true);	
		}	
	}
	
	void on_pushButton_network_clicked()
	{
		int ret = 0, value = -1;
		int dhcp = (Qt::Checked == ui.checkBox_dhcp->checkState()) ? 1 : 0;
		string ip = ui.lineEdit_ip->text().toStdString();
		string netmask = ui.lineEdit_netmask->text().toStdString();
		string gateway = ui.lineEdit_gateway->text().toStdString();
		string mac = ui.lineEdit_mac->text().toStdString();
	
		while(1)
		{
			ret = camera_->set_value("dhcp", dhcp);
			if (ret < 0)
				break;
			
			if (!dhcp)
			{
				ret = camera_->set_value("ip", ip);
				if (ret < 0)
					break;
				
				ret = camera_->set_value("netmask", netmask);
				if (ret < 0)
					break;
				
				ret = camera_->set_value("gateway", gateway);
				if (ret < 0)
					break;
			}	
			
			ret = camera_->set_value("mac", mac);
			if (ret < 0)
				break;
			
			QMessageBox::warning(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("网络参数设置成功，重启设备生效"));
			return;
		}	
		
		QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("网络参数设置失败"));
		
	}
	
	 
	void on_pushButton_record_path_clicked()
	{
		QFileDialog dialog(this);
		dialog.setFileMode(QFileDialog::DirectoryOnly);

		if (dialog.exec())
		{
			QString path_name = tr(dialog.selectedFiles().at(0).toUtf8());
			ui.lineEdit_record_path->setText(path_name);
			record_count_ = 0;
			screenshot_count_ = 0;
		}
	} 

	void on_pushButton_record_clicked()
	{
		int ret;
		if (save_avi_ == 0)
		{
			stringstream os;
			os << ui.lineEdit_record_path->text().toStdString() << "/record_" << record_count_ << ".avi";
			avi_name_ = os.str();
			record_count_++;
			record_frame_count_ = 0;
			ui.pushButton_record->setText(QString::fromLocal8Bit("停止录制"));

			save_avi_ = 1;
		}
		else
		{
			mrecord.close_media();
			save_avi_ = 0;
			
			ui.pushButton_record->setText(QString::fromLocal8Bit("录制"));	
		}
	}

	void on_pushButton_screenshot_clicked()
	{
		stringstream os;
		os << ui.lineEdit_record_path->text().toStdString() << "/picture_" << screenshot_count_ << ".jpg";
		pic_name_ = os.str();
		screenshot_count_++;
		save_pic_ = 1;

	}
	
	void on_checkBox_show_mot_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				show_mot_ = 0;
				break;
			case Qt::Checked:
				show_mot_ = 1;
				break;
		}
	}
	
	void on_checkBox_show_sot_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				show_sot_ = 0;
				break;
			case Qt::Checked:
				show_sot_ = 1;
				break;
		}
	}
	
	
	void on_checkBox_graph_coord_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				show_detect_box_mask_ &= ~STEREO_GRAPH_COORD_MASK;
				break;
			case Qt::Checked:
				show_detect_box_mask_ |= STEREO_GRAPH_COORD_MASK;
				break;
		}
	}
	
	void on_checkBox_depth_coord_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				show_detect_box_mask_ &= ~STEREO_DEPTH_COORD_MASK;
				break;
			case Qt::Checked:
				show_detect_box_mask_ |= STEREO_DEPTH_COORD_MASK;
				break;
		}
	}
	
	void on_checkBox_camera_coord_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				show_detect_box_mask_ &= ~STEREO_CAMERA_COORD_MASK;
				break;
			case Qt::Checked:
				show_detect_box_mask_ |= STEREO_CAMERA_COORD_MASK;
				break;
		}
	}
	
	void on_checkBox_room_coord_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				show_detect_box_mask_ &= ~STEREO_ROOM_COORD_MASK;
				break;
			case Qt::Checked:
				show_detect_box_mask_ |= STEREO_ROOM_COORD_MASK;
				break;
		}
	}
	
	void on_checkBox_ball_coord_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				show_detect_box_mask_ &= ~STEREO_BALL_COORD_MASK;
				break;
			case Qt::Checked:
				show_detect_box_mask_ |= STEREO_BALL_COORD_MASK;
				break;
		}
	}
	
	void on_checkBox_ptz_coord_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				show_detect_box_mask_ &= ~STEREO_PTZ_COORD_MASK;
				break;
			case Qt::Checked:
				show_detect_box_mask_ |= STEREO_PTZ_COORD_MASK;
				break;
		}
	}
	
	void on_checkBox_show_cursor_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				show_cursor_mode_ = 0;
				break;
			case Qt::Checked:
				show_cursor_mode_ = 1;
				break;
		}
	}
	
	void on_pushButton_update_clicked();
	
	//ptz
	void on_pushButton_open_ptz_clicked();
	void on_comboBox_ptz_track_mode_currentIndexChanged(int index);
	void on_checkBox_ptz_calib_en_stateChanged(int arg1); 
	void on_pushButton_clear_ptz_samples_clicked();
	void on_pushButton_add_ptz_sample_clicked();
	void on_pushButton_set_ptz_samples_clicked();
	


	void on_checkBox_datascreen_stateChanged(int arg1)
	{
		switch (arg1)
		{
		case Qt::Unchecked:
			xptz_->set_datascreen_off();
			break;
		case Qt::Checked:
			xptz_->set_datascreen_on();
			break;
		}
	}
	
	void on_toolButton_UL_pressed()
	{
		int pan_speed = ui.spinBox_pan_speed->value();
		int tilt_speed = ui.spinBox_tilt_speed->value();
		xptz_->set_pantilt_upleft(pan_speed, tilt_speed);
	}
	
	void on_toolButton_U_pressed()
	{
		int pan_speed = ui.spinBox_pan_speed->value();
		int tilt_speed = ui.spinBox_tilt_speed->value();
		xptz_->set_pantilt_up(pan_speed, tilt_speed);
	}
	
	void on_toolButton_UR_pressed()
	{
		int pan_speed = ui.spinBox_pan_speed->value();
		int tilt_speed = ui.spinBox_tilt_speed->value();
		xptz_->set_pantilt_upright(pan_speed, tilt_speed);
	}
	
	void on_toolButton_L_pressed()
	{
		int pan_speed = ui.spinBox_pan_speed->value();
		int tilt_speed = ui.spinBox_tilt_speed->value();
		xptz_->set_pantilt_left(pan_speed, tilt_speed);
	}
	
	void on_toolButton_R_pressed()
	{
		int pan_speed = ui.spinBox_pan_speed->value();
		int tilt_speed = ui.spinBox_tilt_speed->value();
		xptz_->set_pantilt_right(pan_speed, tilt_speed);
	}
	
	void on_toolButton_DL_pressed()
	{
		int pan_speed = ui.spinBox_pan_speed->value();
		int tilt_speed = ui.spinBox_tilt_speed->value();
		xptz_->set_pantilt_downleft(pan_speed, tilt_speed);
	}
	
	void on_toolButton_D_pressed()
	{
		int pan_speed = ui.spinBox_pan_speed->value();
		int tilt_speed = ui.spinBox_tilt_speed->value();
		xptz_->set_pantilt_down(pan_speed, tilt_speed);
	}
	
	void on_toolButton_DR_pressed()
	{
		int pan_speed = ui.spinBox_pan_speed->value();
		int tilt_speed = ui.spinBox_tilt_speed->value();
		xptz_->set_pantilt_downright(pan_speed, tilt_speed);
	}
	
	void toolButton_pantilt_stop()
	{
		xptz_->set_pantilt_stop();
	}
	
	void on_toolButton_tele_pressed()
	{
		int zoom_speed = ui.spinBox_zoom_speed->value();
		xptz_->set_zoom_tele(zoom_speed);
	}
	
	void on_toolButton_wide_pressed()
	{
		int zoom_speed = ui.spinBox_zoom_speed->value();
		xptz_->set_zoom_wide(zoom_speed);
	}
	
	void toolButton_zoom_stop()
	{
		xptz_->set_zoom_stop();
	}
	
	void on_spinBox_pan_speed_valueChanged(int value)
	{
		xptz_->set_pan_speed(value);
	}
	
	void on_spinBox_tilt_speed_valueChanged(int value)
	{
		xptz_->set_tilt_speed(value);
	}
	
	void on_spinBox_zoom_speed_valueChanged(int value)
	{
		xptz_->set_zoom_speed(value);
	}
	
	
	void on_spinBox_pan_abs_valueChanged(int value)
	{
		int pan_speed = ui.spinBox_pan_speed->value();
		int tilt_speed = ui.spinBox_tilt_speed->value();
		int pan_abs = ui.spinBox_pan_abs->value();
		int tilt_abs = ui.spinBox_tilt_abs->value();
		xptz_->set_pantilt_absolute_position(pan_abs, tilt_abs, pan_speed, tilt_speed);
	}
	
	void on_spinBox_tilt_abs_valueChanged(int value)
	{
		int pan_speed = ui.spinBox_pan_speed->value();
		int tilt_speed = ui.spinBox_tilt_speed->value();
		int pan_abs = ui.spinBox_pan_abs->value();
		int tilt_abs = ui.spinBox_tilt_abs->value();
		xptz_->set_pantilt_absolute_position(pan_abs, tilt_abs, pan_speed, tilt_speed);
	}
	
	void on_spinBox_zoom_abs_valueChanged(int value)
	{
		int zoom_speed = ui.spinBox_zoom_speed->value();
		int zoom_abs = ui.spinBox_zoom_abs->value();
		xptz_->set_zoom_absolute_position(zoom_abs, zoom_speed);
	}
	
	
	
	
	
	void on_spinBox_min_number_count_valueChanged(int value)
	{
		xfilter_->set_min_number_count(value);
	}
	
	void on_spinBox_max_number_count_valueChanged(int value)
	{
		xfilter_->set_max_number_count(value);
	}
	
	void on_doubleSpinBox_stable_distance_valueChanged(double value)
	{
		xfilter_->set_stable_distance((float)value);
	}
	
	void on_spinBox_min_stable_count_valueChanged(int value)
	{
		xfilter_->set_min_stable_count(value);
	}
	
	void on_comboBox_pid_channel_currentIndexChanged(int index)
	{
		int channel = index;
		ui.doubleSpinBox_pid_kp->setValue((double)xtrack_->get_kp(channel));
		ui.doubleSpinBox_pid_ki->setValue((double)xtrack_->get_ki(channel));
		ui.doubleSpinBox_pid_kd->setValue((double)xtrack_->get_kd(channel));
		ui.doubleSpinBox_pid_dead_zone->setValue((double)xtrack_->get_dead_zone(channel));
		ui.doubleSpinBox_pid_max_limit->setValue((double)xtrack_->get_max_limit(channel));
	}
 
	void on_doubleSpinBox_pid_kp_valueChanged(double value)
	{
		int channel = ui.comboBox_pid_channel->currentIndex();
		xtrack_->set_kp(channel, (float)value);
	}
	
	void on_doubleSpinBox_pid_ki_valueChanged(double value)
	{
		int channel = ui.comboBox_pid_channel->currentIndex();
		xtrack_->set_ki(channel, (float)value);
	}
	
	void on_doubleSpinBox_pid_kd_valueChanged(double value)
	{
		int channel = ui.comboBox_pid_channel->currentIndex();
		xtrack_->set_kd(channel, (float)value);
	}
	
	void on_doubleSpinBox_pid_dead_zone_valueChanged(double value)
	{
		int channel = ui.comboBox_pid_channel->currentIndex();
		xtrack_->set_dead_zone(channel, (float)value);
	}
	
	void on_doubleSpinBox_pid_max_limit_valueChanged(double value)
	{
		int channel = ui.comboBox_pid_channel->currentIndex();
		xtrack_->set_max_limit(channel, (float)value);
	}
};

#endif // ZSCAM_CLIENT_H
