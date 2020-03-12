#ifndef EXFATDLG_H
#define EXFATDLG_H

#include "exfatModel.h"
#include <QtGui/QMainWindow>
#include <QtGui>
#include "ui_exfatdlg.h"

class exfatDlg : public QMainWindow
{
	Q_OBJECT

public:
	exfatDlg(QWidget *parent = 0, Qt::WFlags flags = 0);
	~exfatDlg();

private:
	Ui::exfatDlgClass ui;

	ExfatModel eModel;
	QFileSystemModel InModel;
};

#endif // EXFATDLG_H
