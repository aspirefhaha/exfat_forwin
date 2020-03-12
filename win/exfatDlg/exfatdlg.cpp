#include "exfatdlg.h"

#include <QtGui>
#ifdef _DEBUG
#pragma comment(lib,"libexfat.lib")
#else
#pragma comment(lib,"libexfat.lib")
#endif
exfatDlg::exfatDlg(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags),eModel(this)
{
	ui.setupUi(this);
	//InModel.setRootPath("C:");
	eModel.addRootDevice(QString("fhaha.img"),EXFTDRIVE);
	//ui.tv_main->setModel(&InModel);
	//ui.tv_main->setRootIndex(InModel.index("C:"));
	ui.tv_main->setModel(&eModel);
}

exfatDlg::~exfatDlg()
{

}
