#ifndef ZSCAM_CLIENT_H
#define ZSCAM_CLIENT_H


#include <iostream>
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
#include <opencv2/opencv.hpp>


#include "ui_zscam_client.h"


//#include "camera_command.h"
//#include "system_command.h"

using namespace std;
using namespace cv;


/*
#define SHOW_GRAPH_COORD_INFO_EN		(1 << 0)
#define SHOW_CAMERA_COORD_INFO_EN		(1 << 1)
#define SHOW_ROOM_COORD_INFO_EN			(1 << 2)
#define SHOW_BALL_COORD_INFO_EN			(1 << 3)

#define PTZ_TRACK_PAN_EN				(1 << 0)
#define PTZ_TRACK_TILT_EN				(1 << 1)
#define PTZ_TRACK_ZOOM_EN				(1 << 2)
#define PTZ_TRACK_PTZ_ALL_MASK			(PTZ_TRACK_PAN_EN | PTZ_TRACK_TILT_EN | PTZ_TRACK_ZOOM_EN)

enum ZSCAM_MOUSE_MODE_TYPE
{
	MOUSE_MODE_DEFAULT = 0,
	MOUSE_MODE_POLY_MASK = 1,
};*/



class zscam_client : public QWidget
{
	Q_OBJECT

public:
	zscam_client(QWidget *parent = 0);
	~zscam_client();
	
	


/*	
private:
	void connect_signals();
//	void open_stream();
	void init_zscam_camera_ui();
	void init_zscam_system_ui();
	
	static void handle_stream(void *arg);
	static void handle_discovery(void *arg);
		
	void show_target_boxes(QPixmap *dst, vector<struct http_output_detect_box> &target_boxes);
	string gen_target_boxes_str(struct http_output_detect_box &detect_box, int mask);
	void show_center_cursor(QPixmap *dst, int x, int y);
	void show_mouse_press_point(QPixmap *dst, int x, int y);
	void show_poly_mask(QPixmap *dst, vector<pair<int, int> > &points, QColor color);
	void show_poly_mask_points(QPixmap *dst, vector<pair<int, int> > &points, QColor color);
*/		
public:
	Ui::zscam_clientClass ui;
/*	
	int width;
	int height;
	int detect_mode;
	int track_mode;

	
	record_media mrecord;
	char avifilename[2048];
	char screenshot_name[2048];
	int toggleAvi;
	int toggleJpeg;
	int record_width;
	int record_height;
	int record_count;
	int screenshot_count;
	int record_frame_count;
	boost::posix_time::ptime record_start;
	boost::posix_time::ptime record_end;
	boost::posix_time::millisec_posix_time_system_config::time_duration_type record_time;
	
	int going;
	stream_receiver *mstream;
	boost::mutex mtx_;
	vector<unsigned char> frame_buffer_;
	vector<struct http_output_detect_box> detect_boxes_;
	struct gyro_status gyro_angle_;
//	boost::shared_ptr<boost::thread> stream_thread_;
	std::thread *run_thread_;
	
	camera_command *ccmd;
	system_command *scmd;
	discovery_receiver msearch;
	vector<string> device_nodes_;

	int mouse_mode;
	int cx;
	int cy;
	int mouse_prex;
	int mouse_prey;
	int is_show_cursor;
	int target_show_mask;
	int is_show_poly_mask;
	vector<pair<int, int> > poly_mask_points;
	vector<pair<int, int> > fresh_poly_mask_points;
	*/
	
/*	
signals:
	void close_stream();
	void fresh_frame();	
	void fresh_device_nodes();
*/	
	
	
/*	
private slots:

	void do_close_stream();
	void do_fresh_frame();
	void do_fresh_device_nodes();
	void do_video_label_mouse_pressed(int x, int y);
	void on_pushButton_open_camera_clicked();
	void on_comboBox_http_out_mode_currentIndexChanged(int index)
	{
		if (ccmd)
			ccmd->set_value("set_http_out_mode", index);
	}
	
	void on_radioButton_channel0_mode_clicked()
	{
		if (ccmd)
			ccmd->set_value("set_http_out_channel", 0);
	}

	void on_radioButton_channel1_mode_clicked()
	{
		if (ccmd)
			ccmd->set_value("set_http_out_channel", 1);
	}
	
	void on_spinBox_stream_quality_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_http_out_quality", arg1.toInt());
	}
	

	void on_spinBox_match_edge_th_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_match_edge_th", arg1.toInt());
		
	}
		
	void on_spinBox_match_P1_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_match_P1", arg1.toInt());
	}
		
	void on_spinBox_match_P2_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_match_P2", arg1.toInt());
	}
		
	void on_spinBox_match_check_th_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_match_check_th", arg1.toInt());
	}
		
	void on_comboBox_match_mode_currentIndexChanged(int index)
	{
		if (ccmd)
			ccmd->set_value("set_match_mode", index);
	}
		

	void on_spinBox_bg_color_dist_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_bg_color_dist", arg1.toInt());
	}
		
	void on_spinBox_bg_color_match_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_bg_color_match", arg1.toInt());
	}
		
	void on_spinBox_bg_space_dist_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_bg_space_dist", arg1.toInt());
	}
		
	void on_spinBox_bg_space_match_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_bg_space_match", arg1.toInt());
	}
		
	void on_spinBox_bg_disp_dist_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_bg_disp_dist", arg1.toInt());
	}
		
	void on_spinBox_bg_disp_match_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_bg_disp_match", arg1.toInt());
	}
		
	void on_spinBox_bg_update_radio_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_bg_update_radio", arg1.toInt());
	}
		
	void on_comboBox_bg_mode_currentIndexChanged(int index)
	{
		if (ccmd)
			ccmd->set_value("set_bg_mode", index);
	}
		
	void on_pushButton_bg_init_clicked()
	{
		int value;
		if (ccmd)
			ccmd->get_value("get_bg_init", value);
	}
		

	void on_spinBox_post_tex_th_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_post_tex_th", arg1.toInt());
	}
		
	void on_spinBox_post_tex_sum_th_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_post_tex_sum_th", arg1.toInt());
	}
		
	void on_comboBox_median_mode_currentIndexChanged(int index)
	{
		if (ccmd)
			ccmd->set_value("set_median_mode", index);
	}
		
	void on_comboBox_tex_mode_currentIndexChanged(int index)
	{
		if (ccmd)
			ccmd->set_value("set_tex_mode", index);
	}
	
	void on_comboBox_space_mode_currentIndexChanged(int index)
	{
		if (ccmd)
			ccmd->set_value("set_space_mode", index);
	}

	void on_spinBox_install_height_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_install_height", arg1.toInt());
	}
		
	void on_doubleSpinBox_install_x_angle_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_install_x_angle", (float)arg1.toDouble());
	}
		
	void on_doubleSpinBox_install_z_angle_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_install_z_angle", (float)arg1.toDouble());
	}
		
	void on_spinBox_detect_minx_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_detect_minx", arg1.toInt());
	}
		
	void on_spinBox_detect_maxx_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_detect_maxx", arg1.toInt());
	}
		
	void on_spinBox_detect_miny_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_detect_miny", arg1.toInt());
	}
		
	void on_spinBox_detect_maxy_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_detect_maxy", arg1.toInt());
	}
		
	void on_spinBox_detect_minz_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_detect_minz", arg1.toInt());
	}
		
	void on_spinBox_detect_maxz_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_detect_maxz", arg1.toInt());
	}
		
	void on_comboBox_morph_mode_currentIndexChanged(int index)
	{
		if (ccmd)
			ccmd->set_value("set_morph_mode", index);
	}
	
	void on_comboBox_poly_mask_mode_currentIndexChanged(int index)
	{
		if (ccmd)
			ccmd->set_value("set_poly_mask_mode", index);
	}

	void on_pushButton_detect_mode_clicked()
	{
		if (ccmd)
		{
			if (detect_mode == 0)
			{
				detect_mode = 1;
				ccmd->set_value("set_detect_mode", detect_mode);
				ui.pushButton_detect_mode->setText(QString::fromLocal8Bit("停止检测"));
			}
			else
			{
				detect_mode = 0;
				ccmd->set_value("set_detect_mode", detect_mode);
				ui.pushButton_detect_mode->setText(QString::fromLocal8Bit("开始检测"));
			}	
		}	
			
		
	}

	void on_spinBox_detect_min_wsize_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_detect_min_wsize", arg1.toInt());
	}

	void on_spinBox_detect_min_space_size_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_detect_min_space_size", arg1.toInt());
	}

	void on_spinBox_detect_min_nms_dist_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_detect_min_nms_dist", arg1.toInt());
	}

		
	void on_pushButton_track_mode_clicked()
	{
		if (ccmd)
		{
			if (track_mode == 0)
			{
				track_mode = 1;
				ccmd->set_value("set_track_mode", track_mode);
				ui.pushButton_track_mode->setText(QString::fromLocal8Bit("停止跟踪"));
			}
			else
			{
				track_mode = 0;
				ccmd->set_value("set_track_mode", track_mode);
				ui.pushButton_track_mode->setText(QString::fromLocal8Bit("开始跟踪"));
			}
		}	
			
	}

	void on_spinBox_track_max_cost_valueChanged(const QString &arg1)
	{
		if (ccmd)
			ccmd->set_value("set_track_max_cost", arg1.toInt());
	}
 
	void on_pushButton_save_config_clicked()
	{
		int value;
		if (ccmd)
			ccmd->get_value("get_save_config", value);
	}
	 
	void on_pushButton_record_path_clicked()
	{
		QFileDialog dialog(this);
		dialog.setFileMode(QFileDialog::DirectoryOnly);

		if (dialog.exec())
		{
			QString path_name = tr(dialog.selectedFiles().at(0).toUtf8());
			ui.lineEdit_record_path->setText(path_name);
			record_count = 0;
			screenshot_count = 0;
		}
	}


	void on_pushButton_record_clicked()
	{
		int ret;
		if (toggleAvi == 0)
		{
			sprintf(avifilename, "%s/record_%d.avi", ui.lineEdit_record_path->text().toStdString().c_str(), record_count);
			record_count++;
			ui.pushButton_record->setText(QString::fromLocal8Bit("停止录制"));

			toggleAvi = 1;
		}
		else
		{
			toggleAvi = 0;
			record_end = boost::posix_time::microsec_clock::universal_time();
			record_time = record_end - record_start;
			float fps = (record_frame_count / (float)record_time.total_seconds());
			fprintf(stderr,"setting fps to %f\n",fps);
			mrecord.set_video(record_width, record_height, fps, "MJPG");
			mrecord.close_media();

			ui.pushButton_record->setText(QString::fromLocal8Bit("录制"));	
		}
	}

	void on_pushButton_screenshot_clicked()
	{

		sprintf(screenshot_name, "%s/picture_%d.jpg", ui.lineEdit_record_path->text().toStdString().c_str(), screenshot_count);
		screenshot_count++;
		toggleJpeg = 1;

	}
	
	
	void on_checkBox_graph_coord_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				target_show_mask &= ~SHOW_GRAPH_COORD_INFO_EN;
				break;
			case Qt::Checked:
				target_show_mask |= SHOW_GRAPH_COORD_INFO_EN;
				break;
		}
	}
	
	void on_checkBox_camera_coord_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				target_show_mask &= ~SHOW_CAMERA_COORD_INFO_EN;
				break;
			case Qt::Checked:
				target_show_mask |= SHOW_CAMERA_COORD_INFO_EN;
				break;
		}
	}
	
	void on_checkBox_room_coord_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				target_show_mask &= ~SHOW_ROOM_COORD_INFO_EN;
				break;
			case Qt::Checked:
				target_show_mask |= SHOW_ROOM_COORD_INFO_EN;
				break;
		}
	}
	
	void on_checkBox_ball_coord_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				target_show_mask &= ~SHOW_BALL_COORD_INFO_EN;
				break;
			case Qt::Checked:
				target_show_mask |= SHOW_BALL_COORD_INFO_EN;
				break;
		}
	}
	
	void on_comboBox_mouse_mode_currentIndexChanged(int index)
	{
		mouse_mode = index;
	}
	
	void on_checkBox_show_cursor_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				is_show_cursor = 0;
				break;
			case Qt::Checked:
				is_show_cursor = 1;
				break;
		}
	}
	
	void on_checkBox_show_poly_mask_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				is_show_poly_mask = 0;
				break;
			case Qt::Checked:
				is_show_poly_mask = 1;
				break;
		}
		
		if (ccmd)
			ccmd->get_poly_mask(poly_mask_points);
	}
	
	void on_pushButton_set_poly_mask_clicked()
	{
		if (ccmd)
			ccmd->set_poly_mask(fresh_poly_mask_points);
		
		if (ccmd)
			ccmd->get_poly_mask(poly_mask_points);
	}
	
	void on_pushButton_clear_poly_mask_clicked()
	{
		fresh_poly_mask_points.clear();
	}
	
	
	void on_pushButton_open_system_clicked();
	
	void on_pushButton_reboot_clicked()
	{
		int value;
		if (scmd)
			scmd->get_value("get_reboot", value);
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
		scmd->set_value("set_dhcp", dhcp);
		scmd->set_value("set_ip", ip);
		scmd->set_value("set_netmask", netmask);
		scmd->set_value("set_gateway", gateway);
		ret = scmd->get_value("get_save_network_config", value);
		
		if ((ret != 0) || (value != 0)) {
			QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("网络参数设置失败"));
		} else {
			QMessageBox::warning(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("网络参数设置成功，重启设备生效"));
		}
	}
	
	void on_pushButton_update_clicked();
	void on_checkBox_update_mode_stateChanged(int state)
	{
		switch (state)
		{
			case Qt::Unchecked:
				ui.lineEdit_ftp_host->setEnabled(false);
				ui.lineEdit_ftp_src_file->setEnabled(false);
				ui.lineEdit_ftp_user->setEnabled(false);
				ui.lineEdit_ftp_passwd->setEnabled(false);
				break;
			case Qt::Checked:
				ui.lineEdit_ftp_host->setEnabled(true);
				ui.lineEdit_ftp_src_file->setEnabled(true);
				ui.lineEdit_ftp_user->setEnabled(true);
				ui.lineEdit_ftp_passwd->setEnabled(true);
				break;
		}
	}*/

};

#endif // ZSCAM_CLIENT_H
