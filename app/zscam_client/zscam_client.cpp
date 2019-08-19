#include "zscam_client.h"

zscam_client::zscam_client(QWidget *parent)
	: QWidget(parent)//,
//	msearch("zynq_stereo_camera", 45789, 5000)
{
	ui.setupUi(this);
	/*
	ui.plainTextEdit_all_boxes->setMaximumBlockCount(50);
	ui.plainTextEdit_min_boxes->setMaximumBlockCount(50);
	
	width = 960;
	height = 540;
	
	going = 0;
	mstream = NULL;
	ccmd = NULL;
	scmd = NULL;
	run_thread_ = NULL;
 
	toggleAvi = 0;
	toggleJpeg = 0;
	record_count = 0;
	screenshot_count = 0;
	
	device_nodes_.clear();
	
	mouse_mode = 0;
	cx = 0;
	cy = 0;
	mouse_prex = -1;
	mouse_prey = -1;
	is_show_cursor = 0;
	target_show_mask = SHOW_GRAPH_COORD_INFO_EN;
	is_show_poly_mask = 0;
	poly_mask_points.clear();
	fresh_poly_mask_points.clear();

	boost::thread tA(handle_discovery, this);
	tA.detach();
	connect_signals();*/
}

zscam_client::~zscam_client()
{

}

/*
void zscam_client::on_pushButton_open_camera_clicked() 
{
	int ret;
	string ip = ui.comboBox_ip->currentText().toStdString();
	int stream_number = ui.comboBox_stream_number->currentIndex();
	
	if (!mstream) 
	{
		ui.pushButton_open_camera->setEnabled(false);
		try {
			mstream = new stream_receiver(ip, 9090, stream_number);
		} catch (...) {
			QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("打开失败，请检查设备是否上电或IP填写是否正确"));
			goto OUT_ENABLE_BUTTON;
		}
		if (mstream) {
			ret = mstream->query_frame(frame_buffer_, 5);
			if (ret < 0)
			{
				QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("无法获取视频流"));
				goto OUT_DELETE_MSTREAM;
			}
			
			going = 1;
		//	stream_thread_.reset(new boost::thread(boost::bind(&zscam_client::open_stream, this)));
			run_thread_ = new std::thread(zscam_client::handle_stream, this);
		}

		ccmd = new camera_command(ip.c_str(), 7070);
		
		init_zscam_camera_ui();
 	
		ui.pushButton_open_camera->setText(QString::fromLocal8Bit("关闭"));	
		ui.pushButton_open_camera->setEnabled(true);
		return;
		
OUT_DELETE_MCMD:
		delete ccmd;
		ccmd = NULL;

OUT_DELETE_MSTREAM:
		delete mstream;
		mstream = NULL;
		
OUT_ENABLE_BUTTON:
		ui.pushButton_open_camera->setEnabled(true);
		
		return;
	} 
	else 
	{
		ui.pushButton_open_camera->setEnabled(false);
		delete ccmd;
		ccmd = NULL;
	//	if (stream_thread_)
	//	{
	//		stream_thread_->interrupt();
	//		stream_thread_->join();
	//		stream_thread_.reset();
	//	}
		
		emit close_stream();
	}	
}

//void zscam_client::open_stream()
//{
//	boost::thread t(handle_stream, this);
//	t.join();
//}

void zscam_client::handle_stream(void *arg)
{
	int ret;
	zscam_client *zscam = (zscam_client *)arg;
	vector<unsigned char> frame_buffer;
	vector<struct http_output_detect_box> detect_boxes;
	struct gyro_status gyro_angle;
	memset(&gyro_angle, 0, sizeof(gyro_angle));
	while(zscam->going)
	{
		
		ret = zscam->mstream->query_frame(frame_buffer, detect_boxes, gyro_angle, 5);
		if (ret < 0) {
			break;
		} 
		{
			boost::lock_guard<boost::mutex> lock(zscam->mtx_);
			zscam->frame_buffer_.resize(frame_buffer.size());
			memcpy((char *)&zscam->frame_buffer_[0], (char *)&frame_buffer[0], frame_buffer.size());
			
			zscam->detect_boxes_.clear();
			for (int i = 0; i < detect_boxes.size(); i++)
			{
				zscam->detect_boxes_.push_back(detect_boxes[i]);
			}	
			
			memcpy(&zscam->gyro_angle_, &gyro_angle, sizeof(gyro_angle));
			
			emit zscam->fresh_frame();
		}
		
	}	
	emit zscam->close_stream();
}

void zscam_client::handle_discovery(void *arg)
{
	zscam_client *zscam = (zscam_client *)arg;
	while(1)
	{
		zscam->msearch.query_device_nodes(zscam->device_nodes_);
		emit zscam->fresh_device_nodes();
	}
}

void zscam_client::connect_signals()
{
	connect(this, SIGNAL(close_stream()), this, SLOT(do_close_stream()), Qt::QueuedConnection);
	connect(this, SIGNAL(fresh_frame()), this, SLOT(do_fresh_frame()), Qt::QueuedConnection);
	connect(this, SIGNAL(fresh_device_nodes()), this, SLOT(do_fresh_device_nodes()), Qt::QueuedConnection);

	connect(ui.label_video, SIGNAL(Mouse_Pressed(int, int)), this, SLOT(do_video_label_mouse_pressed(int, int)));
 
}

void zscam_client::do_close_stream()
{
	going = 0;
	if (run_thread_) {
		run_thread_->join();
		delete run_thread_;
		run_thread_ = NULL;
	}
	
	if (mstream) {
		delete mstream;
		mstream = NULL;
	}
	ui.pushButton_open_camera->setText(QString::fromLocal8Bit("连接"));
	ui.pushButton_open_camera->setEnabled(true);
}

void zscam_client::do_fresh_frame()
{
	vector<unsigned char> frame_buffer;
	vector<struct http_output_detect_box> detect_boxes;
	struct gyro_status gyro_angle;
	{
		boost::lock_guard<boost::mutex> lock(mtx_);
		frame_buffer.resize(frame_buffer_.size());
		memcpy((char *)&frame_buffer[0], (char *)&frame_buffer_[0], frame_buffer_.size());
		
		for (int i = 0; i < detect_boxes_.size(); i++)
		{
			detect_boxes.push_back(detect_boxes_[i]);
		}	
		
		memcpy(&gyro_angle, &gyro_angle_, sizeof(gyro_angle));
	}
	
	QPixmap pixmap;
	bool ret = pixmap.loadFromData((const unsigned char *)&frame_buffer[0], frame_buffer.size());
	if (!ret)
		return;

	show_target_boxes(&pixmap, detect_boxes);
	
	if (is_show_poly_mask)
	{
		show_poly_mask(&pixmap, poly_mask_points, Qt::green);
	}
	
	if (mouse_mode == MOUSE_MODE_POLY_MASK)
	{
		show_poly_mask_points(&pixmap, fresh_poly_mask_points, Qt::red);
	}	
	
	ui.doubleSpinBox_gyro_roll->setValue(gyro_angle.roll);
	ui.doubleSpinBox_gyro_pitch->setValue(gyro_angle.pitch);
	
	if (is_show_cursor)
		show_center_cursor(&pixmap, (int)cx, (int)cy);

		
	int min_id = -1;
	int min_n = -1;
	for (int i = 0; i < detect_boxes.size(); i++)
	{
		struct http_output_detect_box &detect_box = detect_boxes[i];
		string str = gen_target_boxes_str(detect_box, target_show_mask);
		ui.plainTextEdit_all_boxes->appendPlainText(QString::fromStdString(str));
		
		int id = detect_box.id;
		if ((min_id < 0) || (id < min_id))
		{
			min_id = id;
			min_n = i;
		}	
	}	
	
	if (min_n >= 0) {
		struct http_output_detect_box &detect_box = detect_boxes[min_n];

		string str = gen_target_boxes_str(detect_box, target_show_mask);
		ui.plainTextEdit_min_boxes->appendPlainText(QString::fromStdString(str));
		
	//	detect_control_ptz_auto(detect_box, ptz_track_mask, ptz_install_mode, 5);
	}

	while (toggleAvi == 1)
	{
		if (!mrecord.isOpened()) {
			ret = mrecord.open_media(avifilename);
			if (ret < 0)
			{
				toggleAvi = 0;
				ui.pushButton_record->setText(QString::fromLocal8Bit("录制"));	
				break;
			}
			record_width = pixmap.width();
			record_height = pixmap.height();		
			mrecord.set_video(record_width, record_height, 15, "MJPG");
			record_start = boost::posix_time::microsec_clock::universal_time();
			record_frame_count = 0;
		}
		
		
		mrecord.write_frame((char *)&frame_buffer[0], frame_buffer.size(), record_frame_count);
		record_frame_count++;
		break;
	}

	if (toggleJpeg)
	{
		toggleJpeg = 0;
		get_picture(screenshot_name, (unsigned char *)&frame_buffer[0], frame_buffer.size());
	}
	
	int w = ui.label_video->width();
	int h = ui.label_video->height();
	QPixmap scale_pixmap = pixmap.scaled(w, h);
	
	if (mouse_mode == MOUSE_MODE_DEFAULT)
		show_mouse_press_point(&scale_pixmap, mouse_prex, mouse_prey);
	
	ui.label_video->setPixmap(scale_pixmap);
}

void zscam_client::do_fresh_device_nodes()
{ 
	for (int i = 0; i < device_nodes_.size(); i++)
	{
		QString ip = QString::fromStdString(device_nodes_[i]);
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
		for (int j = 0; j < device_nodes_.size(); j++)
		{
			QString ip = QString::fromStdString(device_nodes_[j]);
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

void zscam_client::do_video_label_mouse_pressed(int x, int y)
{
	int ret;
	cout << "x: " << x << " y: " << y << endl;
	mouse_prex = x;
	mouse_prey = y;
	
	if (!ccmd)
		return;
	
	float w = ui.label_video->width();
	float h = ui.label_video->height();
	
	int sx = (float)x / w * (float)width;
	int sy = (float)y / h * (float)height;
	
	
	
	switch(mouse_mode)
	{
		case MOUSE_MODE_DEFAULT:
		{
			struct point_space_status point;
			memset(&point, 0, sizeof(point));
			ret = ccmd->get_point_space_status(sx, sy, point);
			if ((ret < 0) || (point.d <= 0))
				return;
			
			struct http_output_detect_box detect_box;
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
			
			string str = gen_target_boxes_str(detect_box, target_show_mask);
			ui.plainTextEdit_all_boxes->appendPlainText(QString::fromStdString(str));
		}break;
		
		case MOUSE_MODE_POLY_MASK:
		{
			pair<int, int> point;
			point.first = sx;
			point.second = sy;
			fresh_poly_mask_points.push_back(point);
		}break;
		
		default:
			break;
		
	}
	
	
	
//	detect_control_ptz(detect_box, ptz_track_mode, ptz_track_mask, ptz_install_mode, 5);
	
}



void zscam_client::init_zscam_camera_ui()
{
	int ret;
	int value;
	float fvalue;
	string svalue;
	
	if (!ccmd)
		return;
	 
	ret = ccmd->get_value("get_cx", fvalue);
	if (ret == 0) {
		cx = fvalue;
	}	
	
	ret = ccmd->get_value("get_cy", fvalue);
	if (ret == 0) {
		cy = fvalue;
	}
	 
	ret = ccmd->get_value("get_http_out_mode", value);
	if (ret == 0) {
		ui.comboBox_http_out_mode->setCurrentIndex(value);
	}	

	ret = ccmd->get_value("get_http_out_quality", value);
	if (ret == 0) {
		ui.spinBox_stream_quality->setValue(value);
	}
	
	ret = ccmd->get_value("get_match_edge_th", value);
	if (ret == 0) {
		ui.spinBox_match_edge_th->setValue(value);
	}
	
	ret = ccmd->get_value("get_match_P1", value);
	if (ret == 0) {
		ui.spinBox_match_P1->setValue(value);
	}
	
	ret = ccmd->get_value("get_match_P2", value);
	if (ret == 0) {
		ui.spinBox_match_P2->setValue(value);
	}
		
	ret = ccmd->get_value("get_match_check_th", value);
	if (ret == 0) {
		ui.spinBox_match_check_th->setValue(value);
	}
		
	ret = ccmd->get_value("get_match_mode", value);
	if (ret == 0) {
		ui.comboBox_match_mode->setCurrentIndex(value);
	}	
		
	ret = ccmd->get_value("get_bg_color_dist", value);
	if (ret == 0) {
		ui.spinBox_bg_color_dist->setValue(value);
	}	
		
	ret = ccmd->get_value("get_bg_color_match", value);
	if (ret == 0) {
		ui.spinBox_bg_color_match->setValue(value);
	}

	ret = ccmd->get_value("get_bg_space_dist", value);
	if (ret == 0) {
		ui.spinBox_bg_space_dist->setValue(value);
	}	
		
	ret = ccmd->get_value("get_bg_space_match", value);
	if (ret == 0) {
		ui.spinBox_bg_space_match->setValue(value);
	}

	ret = ccmd->get_value("get_bg_disp_dist", value);
	if (ret == 0) {
		ui.spinBox_bg_disp_dist->setValue(value);
	}	
		
	ret = ccmd->get_value("get_bg_disp_match", value);
	if (ret == 0) {
		ui.spinBox_bg_disp_match->setValue(value);
	}
	
	ret = ccmd->get_value("get_bg_update_radio", value);
	if (ret == 0) {
		ui.spinBox_bg_update_radio->setValue(value);
	}	
		
	ret = ccmd->get_value("get_bg_mode", value);
	if (ret == 0) {
		ui.comboBox_bg_mode->setCurrentIndex(value);
	}	
	
	ret = ccmd->get_value("get_install_height", value);
	if (ret == 0) {
		ui.spinBox_install_height->setValue(value);
	}
	
	ret = ccmd->get_value("get_install_x_angle", fvalue);
	if (ret == 0) {
		ui.doubleSpinBox_install_x_angle->setValue(fvalue);
	}
	
	ret = ccmd->get_value("get_install_z_angle", fvalue);
	if (ret == 0) {
		ui.doubleSpinBox_install_z_angle->setValue(fvalue);
	}
	
	ret = ccmd->get_value("get_detect_minx", value);
	if (ret == 0) {
		ui.spinBox_detect_minx->setValue(value);
	}
	
	ret = ccmd->get_value("get_detect_maxx", value);
	if (ret == 0) {
		ui.spinBox_detect_maxx->setValue(value);
	}
	
	ret = ccmd->get_value("get_detect_miny", value);
	if (ret == 0) {
		ui.spinBox_detect_miny->setValue(value);
	}
	
	ret = ccmd->get_value("get_detect_maxy", value);
	if (ret == 0) {
		ui.spinBox_detect_maxy->setValue(value);
	}
	
	ret = ccmd->get_value("get_detect_minz", value);
	if (ret == 0) {
		ui.spinBox_detect_minz->setValue(value);
	}
	
	ret = ccmd->get_value("get_detect_maxz", value);
	if (ret == 0) {
		ui.spinBox_detect_maxz->setValue(value);
	}
	
	ret = ccmd->get_value("get_post_tex_th", value);
	if (ret == 0) {
		ui.spinBox_post_tex_th->setValue(value);
	}
	
	ret = ccmd->get_value("get_post_tex_sum_th", value);
	if (ret == 0) {
		ui.spinBox_post_tex_sum_th->setValue(value);
	}
	
	ret = ccmd->get_value("get_median_mode", value);
	if (ret == 0) {
		ui.comboBox_median_mode->setCurrentIndex(value);
	}
	
	ret = ccmd->get_value("get_tex_mode", value);
	if (ret == 0) {
		ui.comboBox_tex_mode->setCurrentIndex(value);
	}
	
	ret = ccmd->get_value("get_space_mode", value);
	if (ret == 0) {
		ui.comboBox_space_mode->setCurrentIndex(value);
	}
	
	ret = ccmd->get_value("get_morph_mode", value);
	if (ret == 0) {
		ui.comboBox_morph_mode->setCurrentIndex(value);
	}
	
	ret = ccmd->get_value("get_detect_mode", value);
	if (ret == 0) {
		detect_mode = value;
		if (detect_mode == 0) {
			ui.pushButton_detect_mode->setText(QString::fromLocal8Bit("开始检测"));
		} else {
			ui.pushButton_detect_mode->setText(QString::fromLocal8Bit("停止检测"));
		}
	}

	ret = ccmd->get_value("get_detect_min_wsize", value);
	if (ret == 0) {
		ui.spinBox_detect_min_wsize->setValue(value);
	}

	ret = ccmd->get_value("get_detect_min_space_size", value);
	if (ret == 0) {
		ui.spinBox_detect_min_space_size->setValue(value);
	}

	

	ret = ccmd->get_value("get_track_max_cost", value);
	if (ret == 0) {
		ui.spinBox_track_max_cost->setValue(value);
	}
	
	ret = ccmd->get_value("get_detect_min_nms_dist", value);
	if (ret == 0) {
		ui.spinBox_detect_min_nms_dist->setValue(value);
	}

	ret = ccmd->get_value("get_track_mode", value);
	if (ret == 0) {
		track_mode = value;
		if (track_mode == 0) {
			ui.pushButton_track_mode->setText(QString::fromLocal8Bit("开始跟踪"));
		} else {
			ui.pushButton_track_mode->setText(QString::fromLocal8Bit("停止跟踪"));
		}
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
	if ((x >= 0) && (y >= 0))
	{
		QPainter painter(dst);
		painter.setPen(QPen(Qt::darkBlue, 2, Qt::SolidLine));
		painter.drawRoundRect(x, y, 4, 4);
	}
}

void zscam_client::show_target_boxes(QPixmap *dst, vector<struct http_output_detect_box> &target_boxes)
{
	QPainter painter(dst);
		 
	int box_num = target_boxes.size();
	for (int i = 0; i < box_num; i++)
	{
		struct http_output_detect_box &detect_box = target_boxes[i];
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
		string str = gen_target_boxes_str(detect_box, target_show_mask);
		painter.drawText(box_x, box_y, QString::fromStdString(str));
	}	
}

string zscam_client::gen_target_boxes_str(struct http_output_detect_box &detect_box, int mask)
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

void zscam_client::show_poly_mask(QPixmap *dst, vector<pair<int, int> > &points, QColor color)
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

void zscam_client::show_poly_mask_points(QPixmap *dst, vector<pair<int, int> > &points, QColor color)
{
	QPainter painter(dst);
	for (int i = 0; i < points.size(); i++)
	{
		painter.setPen(QPen(color, 1.5, Qt::SolidLine));
		painter.drawLine(points[i].first - 5, points[i].second, points[i].first + 5, points[i].second);
		painter.drawLine(points[i].first, points[i].second - 5, points[i].first, points[i].second + 5);
	}

}


void zscam_client::on_pushButton_open_system_clicked()
{
	string ip = ui.comboBox_ip->currentText().toStdString();
	if (scmd == NULL) {
		scmd = new system_command(ip.c_str(), 7071);
		init_zscam_system_ui();
		ui.pushButton_open_system->setText(QString::fromLocal8Bit("关闭"));	
	} else {
		delete scmd;
		scmd = NULL;
		ui.pushButton_open_system->setText(QString::fromLocal8Bit("连接"));	
	}
}

void zscam_client::init_zscam_system_ui()
{
	int ret;
	int value;
	float fvalue;
	string svalue;
	
	if (!scmd)
		return;
	
	ret = scmd->get_value("get_version", svalue);
	if (ret < 0)
		return;
	ui.lineEdit_version->setText(QString::fromStdString(svalue));
	
	ret = scmd->get_value("get_serial_number", svalue);
	if (ret < 0)
		return;
	ui.lineEdit_serial_number->setText(QString::fromStdString(svalue));
	
	scmd->get_value("get_load_network_config", value);
	
	ret = scmd->get_value("get_dhcp", value);
	if (ret == 0) {
		if (value)
		{
			ui.checkBox_dhcp->setCheckState(Qt::Checked);
			ui.lineEdit_ip->setEnabled(false);
			ui.lineEdit_netmask->setEnabled(false);
			ui.lineEdit_gateway->setEnabled(false);	
		}	
		else
		{
			ui.checkBox_dhcp->setCheckState(Qt::Unchecked);
			ui.lineEdit_ip->setEnabled(true);
			ui.lineEdit_netmask->setEnabled(true);
			ui.lineEdit_gateway->setEnabled(true);	
			
			string ip, netmask, gateway;
			ret = scmd->get_value("get_ip", ip);
			if (ret == 0) {
				ui.lineEdit_ip->setText(QString::fromStdString(ip));
			}
			
			ret = scmd->get_value("get_netmask", netmask);
			if (ret == 0) {
				ui.lineEdit_netmask->setText(QString::fromStdString(netmask));
			}
			
			ret = scmd->get_value("get_gateway", gateway);
			if (ret == 0) {
				ui.lineEdit_gateway->setText(QString::fromStdString(gateway));
			}
		}	
	}
}


void zscam_client::on_pushButton_update_clicked()
{
	int ret;
	int isOk;
	int update_mode = (ui.checkBox_update_mode->checkState() == Qt::Checked) ? 1 : 0;
	int force = (ui.checkBox_update_force->checkState() == Qt::Checked) ? 1 : 0;
	
	ret = scmd->get_value("get_kill_stereo_streamer", isOk);
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
		
		ret = scmd->set_value("set_update_system", force);
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
		
		ret = scmd->download_update_system(ftp_host, ftp_src_file, ftp_user, ftp_passwd, force);
		if (ret < 0) {
			QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("发送升级命令失败"));
			return;
		}
	}	
    
	 
	update_dialog update_gui(scmd);
	update_gui.exec();
	isOk = update_gui.get_success();
	if (!isOk) {
		QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("升级失败"));
		return;
	}
	QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("升级成功，请重启设备"));
}
*/


