#include "exfatdlg.h"
#include <Windows.h>
#include <QtGui/QApplication>
#include "exnotepad.h"

int main(int argc, char *argv[])
{
	
	QApplication a(argc, argv);
	QTranslator translator; 
    translator.load(QString(":/qm/zh_CN.qm")); 
	a.installTranslator(&translator);
	HANDLE m_hMutex = NULL;
 
	m_hMutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, "AJISWifiShareTool");
	if (m_hMutex == NULL) {
		m_hMutex = CreateMutexA(NULL, TRUE, "AJISWifiShareTool");
	}
	else {
		QMessageBox::warning(NULL, "warning", QString::fromLocal8Bit("冲突服务exfatserver已经启动，本程序无法运行"), QMessageBox::Abort , QMessageBox::Abort);
		//QMessageBox(NULL,"","ERROR",MB_OK);
		return -1;
	}
	exfatDlg w;

	w.setLocale(QLocale("zh_CN.UTF-8"));
	w.show();
	
	return a.exec();
}
