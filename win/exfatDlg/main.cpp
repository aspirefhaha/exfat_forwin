#include "exfatdlg.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTranslator translator;  
    translator.load(QString(":/qm/zh_CN.qm")); 
	a.installTranslator(&translator);
	exfatDlg w;
	w.setLocale(QLocale("zh_CN.UTF-8"));
	w.show();
	return a.exec();
}
