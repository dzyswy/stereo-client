#include "video_qlabel.h"




video_qlabel::video_qlabel(QWidget *parent) : QLabel(parent)
{
	
}

void video_qlabel::mousePressEvent(QMouseEvent *ev)
{
	int x = ev->x();
	int y = ev->y();
	emit Mouse_Pressed(x, y);
}
