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

	ExfatModel exfatModel;
	QFileSystemModel InModel;
	void editFile(const QModelIndex &);

protected slots:
	void sltFormat();
	void sltEdit(bool);
	void sltEdit(const QModelIndex &);
	void sltAddDir(bool);
	void sltAddFile(bool);
	void sltProperties(bool);
	void sltContextMenu(const QPoint &);
	void dragEnterEvent(QDragEnterEvent *);
	void dropEvent(QDropEvent * event);

};

#endif // EXFATDLG_H
