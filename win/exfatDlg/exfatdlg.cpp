#include "exfatdlg.h"

#include <QtGui>

exfatDlg::exfatDlg(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags),eModel(this)
{
	ui.setupUi(this);
	InModel.setRootPath("C:");
	ui.tv_main->setModel(&InModel);
	ui.tv_main->setRootIndex(InModel.index("C:"));
}

exfatDlg::~exfatDlg()
{

}
