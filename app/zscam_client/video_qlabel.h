#ifndef __VIDEO_QLABEL_H
#define __VIDEO_QLABEL_H


#include <QLabel>
#include <QMouseEvent>
#include <QEvent>
#include <QDebug>


class video_qlabel : public QLabel
{
	Q_OBJECT
public:
	explicit video_qlabel(QWidget *parent = 0);
	
	void mousePressEvent(QMouseEvent *ev);
	
signals:
	void Mouse_Pressed(int x, int y);
	
	
	
};












#endif

