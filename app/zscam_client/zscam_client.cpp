﻿#include "zscam_client.h"




zscam_client::zscam_client(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.plainTextEdit_all_boxes->setMaximumBlockCount(50);
	ui.plainTextEdit_min_boxes->setMaximumBlockCount(50);
	
	
	width = 960;
	height = 540;
	going = 0;
	cx_ = width / 2;
	cy_ = height / 2;
	mouse_prex_ = width / 2;
	mouse_prey_ = height / 2;
	
	draw_poly_mask_mode_ = 0;
	show_cursor_mode_ = 0;
	show_detect_box_mask_ = SHOW_GRAPH_COORD_INFO_EN;
	
	
	show_poly_mask_mode_ = 0; 
	poly_mask_points_[0].clear();
	poly_mask_points_[1].clear();
	
	default_pixmap_ = QPixmap(width, height);
	
	save_avi_ = 0;
	save_pic_ = 0;
	record_count_ = 0;
	screenshot_count_ = 0;
	record_frame_count_ = 0;
	
	open_ = 0;
	run_thread_ = NULL;
	
	
	camera_ = new stereo_camera("zynq_stereo_camera", 45789, 5, 0);
	timer_slow_ = new QTimer(this);
	connect(timer_slow_, SIGNAL(timeout()), this, SLOT(do_timer_slow_timeout()));
	timer_slow_->start(2500);
	
	
	connect(this, SIGNAL(fresh_frame_signal()), this, SLOT(do_fresh_frame()), Qt::QueuedConnection);
	connect(ui.label_video, SIGNAL(Mouse_Pressed(int, int)), this, SLOT(do_video_label_mouse_pressed(int, int)));
 
}

zscam_client::~zscam_client()
{
	timer_slow_->stop();
	delete timer_slow_;
	delete camera_;
}


 
void zscam_client::on_pushButton_open_camera_clicked() 
{
	int ret;
	string ip = ui.comboBox_ip->currentText().toStdString();
	int index = ui.comboBox_stream_number->currentIndex();

	if (!open_)
	{
		ret = camera_->open_device(ip.c_str(), 7070, 9090, index);
		if (ret < 0) {
			QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("打开失败，请检查设备是否上电或IP填写是否正确"));			
			return;
		}
		
		open_ = 1;
		run_thread_ = new std::thread([this] () {stream_process();});
		init_ui();
		ui.pushButton_open_camera->setText(QString::fromLocal8Bit("关闭"));
		
	}	
	else
	{
		open_ = 0;
		if (run_thread_)
		{
			run_thread_->join();
			delete run_thread_;
			run_thread_ = NULL;
		}
		camera_->close_device();
		ui.pushButton_open_camera->setText(QString::fromLocal8Bit("连接"));
	}	
	
}

void zscam_client::stream_process()
{
	std::vector<unsigned char> frame_buffer;
	std::vector<struct stereo_detect_box> detect_boxes;
	struct stereo_gyro_angle gyro_angle;
	
	cout << "create client stream thread!\n";
	while(open_)
	{
		int ret = camera_->query_frame(5);
		if (ret < 0) {
			std::this_thread::sleep_for (std::chrono::seconds(1));
			continue;
		}
		camera_->get_frame(frame_buffer);
		camera_->get_detect_boxes(detect_boxes);
		camera_->get_gyro_angle(gyro_angle);
		

		if (save_avi_ == 1)
		{
			QPixmap pixmap;
			bool ok = pixmap.loadFromData((const unsigned char *)&frame_buffer[0], frame_buffer.size());
			if (ok) {
				ret = mrecord.open_media(avi_name_.c_str(), pixmap.width(), pixmap.height(), 15);
				if (ret < 0) {
					save_avi_ = 2;
				} else {
					save_avi_ = 3;
				}
			} else {
				ok = 2;
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
		 
		{
			std::unique_lock<std::mutex> lock(mux_);
			frame_buffer_ = frame_buffer;
			detect_boxes_ = detect_boxes;
			gyro_angle_ = gyro_angle; 
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
		bool ok = pixmap.loadFromData((const unsigned char *)&frame_buffer_[0], frame_buffer_.size());
		if (!ok)
			pixmap = default_pixmap_;
		detect_boxes = detect_boxes_;
		gyro_angle = gyro_angle_;
	}
	
	for (int i = 0; i < detect_boxes.size(); i++)
	{
		struct stereo_detect_box &detect_box = detect_boxes[i];
		string str = gen_detect_box_str(detect_box, show_detect_box_mask_);
		ui.plainTextEdit_all_boxes->appendPlainText(QString::fromStdString(str));
	}	
	
	ui.doubleSpinBox_gyro_roll->setValue(gyro_angle.roll);
	ui.doubleSpinBox_gyro_pitch->setValue(gyro_angle.pitch);
	
	
	
	show_detect_boxes(&pixmap, detect_boxes);

	if (show_poly_mask_mode_)
	{
		show_poly_mask(&pixmap, poly_mask_points_[0], Qt::green);
	}
	
	if (draw_poly_mask_mode_)
	{
		show_poly_mask_points(&pixmap, poly_mask_points_[1], Qt::red);
	}

	if (show_cursor_mode_)
	{
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
	vector<string> device_nodes;
	camera_->get_device_nodes(device_nodes);
	
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
	
	if (!open_)
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
	else
	{
		struct stereo_pixel_point point;
		ret = camera_->get_pixel_point(x, y, point);
		if ((ret < 0) || (point.d <= 0))
			return;
		
		struct stereo_detect_box detect_box;
		memset(&detect_box, 0, sizeof(detect_box));
		detect_box.x = point.x;
		detect_box.y = point.y;
		detect_box.d = point.d;
		detect_box.xcm = point.xcm;
		detect_box.ycm = point.ycm;
		detect_box.zcm = point.zcm;
		detect_box.xtcm = point.xtcm;
		detect_box.ytcm = point.ytcm;
		detect_box.ztcm = point.ztcm;
		detect_box.xa = point.xa;
		detect_box.ya = point.ya;
		detect_box.r = point.r;
		
		string str = gen_detect_box_str(detect_box, show_detect_box_mask_);
		ui.plainTextEdit_all_boxes->appendPlainText(QString::fromStdString(str));
	}	
	
//	detect_control_ptz(detect_box, ptz_track_mode, ptz_track_mask, ptz_install_mode, 5);
	
}



void zscam_client::init_ui()
{
	int ret;
	int value;
	float fvalue;
	string svalue;
	
	if (!open_)
		return;
	 
	ret = camera_->get_value("cx", fvalue);
	if (ret == 0) {
		cx_ = fvalue;
	}	
	
	ret = camera_->get_value("cy", fvalue);
	if (ret == 0) {
		cy_ = fvalue;
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
	
	ret = camera_->get_value("detect_mode", value);
	if (ret == 0) {
		detect_mode_ = value;
		if (detect_mode_ == 0) {
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
 
	ret = camera_->get_value("track_max_cost", value);
	if (ret == 0) {
		ui.spinBox_track_max_cost->setValue(value);
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
	
	ret = camera_->get_value("version", svalue);
	if (ret == 0) {
		ui.lineEdit_version->setText(QString::fromStdString(svalue));
	}	
	 
	ret = camera_->get_value("serial_number", svalue);
	if (ret == 0) {
		ui.lineEdit_serial_number->setText(QString::fromStdString(svalue));
	}
 
	ret = camera_->get_value("dhcp", value);
	if (ret == 0) {
		if (value) {
			ui.checkBox_dhcp->setCheckState(Qt::Checked); 
			ui.lineEdit_ip->setEnabled(false);
			ui.lineEdit_netmask->setEnabled(false);
			ui.lineEdit_gateway->setEnabled(false);	
		} else {
			ui.checkBox_dhcp->setCheckState(Qt::Unchecked); 
			ui.lineEdit_ip->setEnabled(true);
			ui.lineEdit_netmask->setEnabled(true);
			ui.lineEdit_gateway->setEnabled(true);
		}	
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
	
}



void zscam_client::show_center_cursor(QPixmap *dst, int x, int y)
{
	if ((x >= 0) && (y >= 0))
	{
		QPainter painter(dst);
		painter.setPen(QPen(Qt::blue, 1.5, Qt::SolidLine));
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
	QPainter painter(dst);
		 
	int box_num = detect_boxes.size();
	for (int i = 0; i < box_num; i++)
	{
		struct stereo_detect_box &detect_box = detect_boxes[i];
		int id = detect_box.id;
		int box_x = detect_box.box_x;
		int box_y = detect_box.box_y;
		int box_w = detect_box.box_w;
		int box_h = detect_box.box_h;
		QColor color = QColor(255, 255, 255);
		if (id > 0) 
		{
			color = QColor((id * 10) % 255, (id * 20) % 255, (id * 30) % 255);
		}	
		
		painter.setPen(QPen(color, 1.5, Qt::SolidLine));
		painter.drawRect(box_x, box_y, box_w, box_h);
		
	//	std::stringstream os;
	//	os << "x=" << detect_box.x << " y=" << detect_box.y << " d=" << detect_box.d;
		painter.setPen(QPen(color, 2, Qt::SolidLine));
		string str = gen_detect_box_str(detect_box, show_detect_box_mask_);
		painter.drawText(box_x, box_y, QString::fromStdString(str));
	}	
}

string zscam_client::gen_detect_box_str(struct stereo_detect_box &detect_box, int mask)
{
	std::stringstream os;
	os << "id=" << detect_box.id << " ";
	if (mask & SHOW_GRAPH_COORD_INFO_EN) {
		os << "box=(" << detect_box.box_x << " " << detect_box.box_y << " " << detect_box.box_w << " " << detect_box.box_h << ") ";
		os << "x=" << detect_box.x << " y=" << detect_box.y << " d=" << detect_box.d << " ";
	}
	
	if (mask & SHOW_CAMERA_COORD_INFO_EN) {
		os << "xcm=" << detect_box.xcm << " ycm=" << detect_box.ycm << " zcm=" << detect_box.zcm << " ";
	}
	
	if (mask & SHOW_ROOM_COORD_INFO_EN) {
		os << "xtcm=" << detect_box.xtcm << " ytcm=" << detect_box.ytcm << " ztcm=" << detect_box.ztcm << " ";
	}
	
	if (mask & SHOW_BALL_COORD_INFO_EN) {
		os << "xa=" << detect_box.xa << " ya=" << -detect_box.ya << " r=" << detect_box.r << " ";
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





/*
void zscam_client::on_pushButton_update_clicked()
{
	int ret;
	int isOk;
	int update_mode = (ui.checkBox_update_mode->checkState() == Qt::Checked) ? 1 : 0;
	int force = (ui.checkBox_update_force->checkState() == Qt::Checked) ? 1 : 0;
	
	ret = camera_->get_value("kill_stereo_streamer", isOk);
	if ((ret < 0) || (isOk < 0)) {
		QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("清理升级环境失败"));
		return;
	}
		
	
	if (update_mode == 0)
	{
		QString src_name = QFileDialog::getOpenFileName(0, QString::fromLocal8Bit("选择文件"), QCoreApplication::applicationDirPath(), QString::fromLocal8Bit("所有文件(*)"));

		QString ftp_ip = ui.comboBox_ip->currentText();
		ftp_dialog dialog(ftp_ip.toStdString().c_str(), src_name.toStdString().c_str(), "/update/ota.bin", "zynq:3g*aAcdV", this);
		dialog.exec();
		
		isOk = dialog.get_success();
		if (!isOk) {
			QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("上传升级包失败"));
			return;
		}
		
		ret = camera_->set_value("set_update_system", force);
		if (ret < 0) {
			QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("发送升级命令失败"));
			return;
		}
	}	
	else
	{
		string ftp_host = ui.lineEdit_ftp_host->text().toStdString();
		string ftp_src_file = ui.lineEdit_ftp_src_file->text().toStdString();
		string ftp_user = ui.lineEdit_ftp_user->text().toStdString();
		string ftp_passwd = ui.lineEdit_ftp_passwd->text().toStdString();
		
		ret = camera_->download_update_system(ftp_host, ftp_src_file, ftp_user, ftp_passwd, force);
		if (ret < 0) {
			QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("发送升级命令失败"));
			return;
		}
	}	
    
	 
	update_dialog update_gui(camera_);
	update_gui.exec();
	isOk = update_gui.get_success();
	if (!isOk) {
		QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("升级失败"));
		return;
	}
	QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("升级成功，请重启设备"));
}
*/


