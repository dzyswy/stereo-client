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
//#include <opencv2/opencv.hpp>


#include "ui_zscam_client.h"
#include "stereo_camera.h"
#include "stereo_detect_boxes.h"
#include "stereo_gyro_angle.h"
#include "stereo_pixel_point.h"
#include "media_record.h"

#define SHOW_GRAPH_COORD_INFO_EN		(1 << 0)
#define SHOW_CAMERA_COORD_INFO_EN		(1 << 1)
#define SHOW_ROOM_COORD_INFO_EN			(1 << 2)
#define SHOW_BALL_COORD_INFO_EN			(1 << 3)

#define PTZ_TRACK_PAN_EN				(1 << 0)
#define PTZ_TRACK_TILT_EN				(1 << 1)
#define PTZ_TRACK_ZOOM_EN				(1 << 2)
#define PTZ_TRACK_PTZ_ALL_MASK			(PTZ_TRACK_PAN_EN | PTZ_TRACK_TILT_EN | PTZ_TRACK_ZOOM_EN)




using namespace std;
//using namespace cv;





 



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
	void stream_process();
	void show_detect_boxes(QPixmap *dst, vector<struct stereo_detect_box> &target_boxes);
	string gen_detect_box_str(struct stereo_detect_box &detect_box, int mask);
	void init_ui();
	void show_center_cursor(QPixmap *dst, int x, int y);
	void show_mouse_press_point(QPixmap *dst, int x, int y);
	void show_poly_mask(QPixmap *dst, vector<pair<float, float> > &points, QColor color);
	void show_poly_mask_points(QPixmap *dst, vector<pair<float, float> > &points, QColor color);
		
public:
	Ui::zscam_clientClass ui;
	int width;
	int height;
	int going;
	stereo_camera *camera_;
	QTimer *timer_slow_;
	media_record xrecord;
	
	int open_;
	QPixmap default_pixmap_;
	std::vector<unsigned char> frame_buffer_;
	std::vector<struct stereo_detect_box> detect_boxes_;
	struct stereo_gyro_angle gyro_angle_;
	std::mutex mux_;
	std::condition_variable cond_;
	std::thread *run_thread_;
	
	float cx_;
	float cy_;
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

 
	int mouse_mode_;
	int show_cursor_mode_;
	int show_detect_box_mask_;
	int show_poly_mask_mode_;
	vector<pair<float, float> > poly_mask_points_[2];
	

	

signals:
	void fresh_frame_signal();	
	
private slots:
	void on_pushButton_open_camera_clicked();
	void do_fresh_frame();
	void do_timer_slow_timeout();
	void do_video_label_mouse_pressed(int x, int y);
	
	
	void on_comboBox_match_mode_currentIndexChanged(int index)
	{
		if (open_) 
			camera_->set_value("match_mode", index);
	}
	 
	void on_spinBox_match_edge_th_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("match_edge_th", arg1.toInt());
		
	}
		
	void on_spinBox_match_P1_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("match_P1", arg1.toInt());
	}
		
	void on_spinBox_match_P2_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("match_P2", arg1.toInt());
	}
		
	void on_spinBox_match_check_th_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("match_check_th", arg1.toInt());
	}
		
	void on_comboBox_bg_mode_currentIndexChanged(int index)
	{
		
		if (open_) 
			camera_->set_value("bg_mode", index);
	}
		 
	void on_spinBox_bg_color_dist_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("bg_color_dist", arg1.toInt());
	}
		
	void on_spinBox_bg_color_match_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("bg_color_match", arg1.toInt());
	}
		
	void on_spinBox_bg_space_dist_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("bg_space_dist", arg1.toInt());
	}
		
	void on_spinBox_bg_space_match_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("bg_space_match", arg1.toInt());
	}
		
	void on_spinBox_bg_disp_dist_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("bg_disp_dist", arg1.toInt());
	}
		
	void on_spinBox_bg_disp_match_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("bg_disp_match", arg1.toInt());
	}
		
	void on_spinBox_bg_update_radio_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("bg_update_radio", arg1.toInt());
	}
		
		
	void on_pushButton_bg_init_clicked()
	{
		int value;
		
		if (open_) 
			camera_->do_action("set_bg_init");
	}
		

		
	void on_comboBox_median_mode_currentIndexChanged(int index)
	{
		
		if (open_) 
			camera_->set_value("median_mode", index);
	}
		
	void on_comboBox_tex_mode_currentIndexChanged(int index)
	{
		
		if (open_) 
			camera_->set_value("tex_mode", index);
	}
	
	void on_comboBox_space_mode_currentIndexChanged(int index)
	{
		
		if (open_) 
			camera_->set_value("space_mode", index);
	}
	
	void on_comboBox_morph_mode_currentIndexChanged(int index)
	{
		
		if (open_) 
			camera_->set_value("morph_mode", index);
	}
	
	void on_spinBox_post_tex_th_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("post_tex_th", arg1.toInt());
	}
		
	void on_spinBox_post_tex_sum_th_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("post_tex_sum_th", arg1.toInt());
	}
	

	void on_spinBox_install_height_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("install_height", arg1.toInt());
	}
		
	void on_doubleSpinBox_install_x_angle_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("install_x_angle", (float)arg1.toDouble());
	}
		
	void on_doubleSpinBox_install_z_angle_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("install_z_angle", (float)arg1.toDouble());
	}
		
	void on_spinBox_detect_minx_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("detect_minx", arg1.toInt());
	}
		
	void on_spinBox_detect_maxx_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("detect_maxx", arg1.toInt());
	}
		
	void on_spinBox_detect_miny_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("detect_miny", arg1.toInt());
	}
		
	void on_spinBox_detect_maxy_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("detect_maxy", arg1.toInt());
	}
		
	void on_spinBox_detect_minz_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("detect_minz", arg1.toInt());
	}
		
	void on_spinBox_detect_maxz_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("detect_maxz", arg1.toInt());
	}
		
	
	
	void on_comboBox_poly_mask_mode_currentIndexChanged(int index)
	{
		
		if (open_) 
			camera_->set_value("poly_mask_mode", index);
	}
	
	void on_pushButton_set_poly_mask_clicked()
	{ 
		if (open_) 
			camera_->set_poly_mask(poly_mask_points_[1]);
		
		if (open_) 
			camera_->get_poly_mask(poly_mask_points_[0]);
	}
	
	void on_pushButton_clear_poly_mask_clicked()
	{
		poly_mask_points_[1].clear();
	}

	void on_pushButton_detect_mode_clicked()
	{
		if (open_)
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
			
		
	}

	void on_spinBox_detect_min_wsize_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("detect_min_wsize", arg1.toInt());
	}

	void on_spinBox_detect_min_space_size_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("detect_min_space_size", arg1.toInt());
	}

	void on_spinBox_detect_min_nms_dist_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("detect_min_nms_dist", arg1.toInt());
	}

	/*	
	void on_pushButton_track_mode_clicked()
	{
		if (open_)
		{
			if (track_mode_ == 0)
			{
				track_mode_ = 1;
				camera_->set_value("track_mode", track_mode_);
				ui.pushButton_track_mode->setText(QString::fromLocal8Bit("停止跟踪"));
			}
			else
			{
				track_mode_ = 0;
				camera_->set_value("track_mode", track_mode_);
				ui.pushButton_track_mode->setText(QString::fromLocal8Bit("开始跟踪"));
			}
		}	
			
	}*/
	
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
		 
	//	if (open_) 
	//		camera_->set_value("track_mode", track_mode_);
	}

	void on_spinBox_track_max_cost_valueChanged(const QString &arg1)
	{
		if (open_) 
			camera_->set_value("track_max_cost", arg1.toInt());
	}
	
	void on_comboBox_http_out_mode_currentIndexChanged(int index)
	{
		
		if (open_) 
			camera_->set_value("http_out_mode", index);
	}
	
	void on_radioButton_channel0_mode_clicked()
	{
		
		if (open_) 
			camera_->set_value("http_out_channel", 0);
	}

	void on_radioButton_channel1_mode_clicked()
	{
		
		if (open_) 
			camera_->set_value("http_out_channel", 1);
	}
	
	void on_spinBox_stream_quality_valueChanged(const QString &arg1)
	{
		
		if (open_) 
			camera_->set_value("http_out_quality", arg1.toInt());
	}
	
	void on_pushButton_save_config_clicked()
	{
		if (open_) 
			camera_->do_action("save_config");
	}
	
	void on_pushButton_reboot_clicked()
	{
		if (open_) 
			camera_->do_action("reboot", 1);
	}
	
	void on_pushButton_network_clicked()
	{
		int ret = 0, value = -1;
		int dhcp = (Qt::Checked == ui.checkBox_dhcp->checkState()) ? 1 : 0;
		string ip = ui.lineEdit_ip->text().toStdString();
		string netmask = ui.lineEdit_netmask->text().toStdString();
		string gateway = ui.lineEdit_gateway->text().toStdString();
		string mac = "";
	
		if (!open_) 
			return;
		
		while(1)
		{
			ret = camera_->set_value("dhcp", dhcp);
			if (ret < 0)
				break;
			
			ret = camera_->set_value("ip", ip);
			if (ret < 0)
				break;
			
			ret = camera_->set_value("netmask", netmask);
			if (ret < 0)
				break;
			
			ret = camera_->set_value("gateway", gateway);
			if (ret < 0)
				break;
			
			ret = camera_->set_value("mac", mac);
			if (ret < 0)
				break;
			
			ret = camera_->do_action("config_network");
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
	
	
	void on_checkBox_graph_coord_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				show_detect_box_mask_ &= ~SHOW_GRAPH_COORD_INFO_EN;
				break;
			case Qt::Checked:
				show_detect_box_mask_ |= SHOW_GRAPH_COORD_INFO_EN;
				break;
		}
	}
	
	void on_checkBox_camera_coord_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				show_detect_box_mask_ &= ~SHOW_CAMERA_COORD_INFO_EN;
				break;
			case Qt::Checked:
				show_detect_box_mask_ |= SHOW_CAMERA_COORD_INFO_EN;
				break;
		}
	}
	
	void on_checkBox_room_coord_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				show_detect_box_mask_ &= ~SHOW_ROOM_COORD_INFO_EN;
				break;
			case Qt::Checked:
				show_detect_box_mask_ |= SHOW_ROOM_COORD_INFO_EN;
				break;
		}
	}
	
	void on_checkBox_ball_coord_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				show_detect_box_mask_ &= ~SHOW_BALL_COORD_INFO_EN;
				break;
			case Qt::Checked:
				show_detect_box_mask_ |= SHOW_BALL_COORD_INFO_EN;
				break;
		}
	}
	
	void on_comboBox_mouse_mode_currentIndexChanged(int index)
	{
		mouse_mode_ = index;
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
		
		
		if (open_) 
			camera_->get_poly_mask(poly_mask_points_[0]);
	}

};

#endif // ZSCAM_CLIENT_H
