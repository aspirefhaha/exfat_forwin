#ifndef EXFATDLG_H
#define EXFATDLG_H

#include "exfatModel.h"
#include <QtGui/QMainWindow>
#include <QtGui>
#include "ui_exfatdlg.h"

#include "BgWorkThread.h"
#include <QProgressDialog>
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
	QProgressDialog dia;
	BgWorkThread bgcopyTh;
	qint64 m_progesssize ;

protected slots:
	void sltFormat();
	void sltLoad();
	void sltEdit(bool);
	void sltEdit(const QModelIndex &);
	void sltAddDir(bool);
	void sltRename(bool);
	void sltDelete(bool);
	void sltExport(bool);
	void sltAddFile(bool);
	void sltProperties(bool);
	void sltContextMenu(const QPoint &);
	void dragEnterEvent(QDragEnterEvent *);
	void dragMoveEvent(QDragMoveEvent *);
	void dropEvent(QDropEvent * event);
	void copyDone(int);
	void updateCpSize(qlonglong);
	void showWarningMsg(QString & ,QString&);
	void updateCpProg(qlonglong);



};

#endif // EXFATDLG_H
