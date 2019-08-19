#include "zscam_client.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	zscam_client w;
	w.show();
	return a.exec();
}
