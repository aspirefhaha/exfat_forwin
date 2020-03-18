#ifndef BGWORKTHREAD_H
#define BGWORKTHREAD_H

#include "exfatModel.h"
#include <QThread>
#include <QPair>

enum BGWORKMODE
{
	WMCALCSIZE=1,
	WMCPTOEXF,
	WMCOPYTOOUTER
};

class BgWorkThread:public QThread
{
	Q_OBJECT

public:
	explicit BgWorkThread(QObject * parent=0);
	~BgWorkThread();
	void setSelPath(QList<QString> &selpaths){
		m_selPaths=selpaths;
	}
	void setTarget(QString &selTarget){
		m_selTargetDir = selTarget;
	}
	void setOutTargetDir(QString tardir){
		m_outtertarget = tardir;
	}
	void setInnerTargetDir(QString &targetdir){
		m_innertarget = targetdir;
	}
	
	size_t getFileDirSize(const char * abspath);
	qlonglong getOutSubItemSize(QString &abspath);
	int getInSubItemCount(QString rootdir,QString curdir,QList<QPair<QString,QString>> &subitems);
	int CopyFilesToExfat(QList<QString> selOutPaths,QString intargetdir);
	int CopyFilesToOuter(QList<QPair<QString,QString>> & copyitems);
	void setWorkMode(BGWORKMODE mode){
		m_bgworkmode = mode;
	}
	void setExfat(exfat * tef){
		ef = tef;
	}
signals:
	void calcSizeRes(unsigned int);
	void calcItemCount(int,int);
	void updateSize(qlonglong);
	void updateProg(qlonglong);
	void warningMsg(QString & ,QString&);
	void copyDone();
protected:
	bool m_bIsQuit;
	QList<QString> m_selPaths;
	void run();

private:
	BGWORKMODE m_bgworkmode ;
	QString m_innertarget;
	QString m_outtertarget;
	exfat * ef;

	QString m_selTargetDir;
	int m_alreadyCopyedCount;
	int m_currentTotalCount;
	quint64 CopyFileToExfat(QString &sourcefile,QString & targetdir,quint64 cursize);
	quint64 CopyDirToExfat(QString &sourcedir,QString & targetdir,quint64 cursize);
};

#endif // BGWORKTHREAD_H
