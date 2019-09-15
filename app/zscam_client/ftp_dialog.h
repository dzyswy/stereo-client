#ifndef __FTP_DIALOG_H
#define __FTP_DIALOG_H


#include <QDialog>
#include <QString>
#include <QDebug>

#include "ui_ftp_dialog.h"
#include "ftp_client.h"


class ftp_dialog : public QDialog
{
	Q_OBJECT
public:
    explicit ftp_dialog(ftp_client *xftp, const char *src_name, const char *dst_name, QWidget *parent = 0);

	void set_process(long send_size, long total_size);
	void set_over(int flag);

	int get_success() {
		return xftp_->get_success();
	}


signals:
	void upload_process(long send_size, long total_size);	
	void upload_over(int flag);
	
public:
	Ui::ftp_dialogClass ui;
	ftp_client *xftp_;

	
private slots:
	void do_upload_process(long send_size, long total_size);
	void do_upload_over(int flag);
	
};














#endif


