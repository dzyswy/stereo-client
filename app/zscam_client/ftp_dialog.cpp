#include "ftp_dialog.h"


using namespace std;


ftp_dialog::ftp_dialog(ftp_client *xftp, const char *src_name, const char *dst_name, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	
	xftp_ = xftp;
	
	connect(this, SIGNAL(upload_over(int)), this, SLOT(do_upload_over(int)), Qt::QueuedConnection);
	connect(this, SIGNAL(upload_process(long, long)),this,SLOT(do_upload_process(long, long)), Qt::QueuedConnection);

	auto xfer_func = [&] (long send_size, long total_size) {
			emit upload_process(send_size, total_size);
			return 0;
		};
	auto over_func = [&] (int flag) {
			emit upload_over(flag);
			return 0;
		};	
	cout << "src_name: " << string(src_name) << endl;
	cout << "dst_name: " << string(dst_name) << endl;	
	xftp->upload(src_name, dst_name, xfer_func, over_func);
	
}

void ftp_dialog::set_process(long send_size, long total_size)
{
	emit upload_process(send_size, total_size);
}

void ftp_dialog::set_over(int flag)
{
	emit upload_over(flag);
}


void ftp_dialog::do_upload_process(long send_size, long total_size)
{
	ui.progressBar->setValue(send_size * 1.0 / total_size * 100);
}

void ftp_dialog::do_upload_over(int flag)
{
	this->close();
}







