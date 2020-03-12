#ifndef EXFATFSMODEL_H
#define EXFATFSMODEL_H
#define __bool_true_false_are_defined 1
extern "C"{
#include "exfat.h"
}
#include <QAbstractItemModel>
#include <qthread.h>
#include <Windows.h>

enum EXFATITEMTYPE {
	EXFTDRIVE=1,
	EXFTSPEC,
	EXFTDIR,
	EXFTFILE,
	EXFTUNKNOWN
};

struct ExfatFSPrivate 
{
	QString absPath;
	EXFATITEMTYPE fstype;
	int m_col;
	int m_row;
	struct exfat * m_pexfatRoot;
	ExfatFSPrivate * m_pParent;
	
	//QList<ExfatFSPrivate *> m_lsChildren;

	bool match(QString name,EXFATITEMTYPE fstype){
		return name==absPath && fstype==this->fstype;
	}

	explicit ExfatFSPrivate(QString devname,EXFATITEMTYPE tt,int row,int col,ExfatFSPrivate * parent)
		:absPath(devname),fstype(tt),m_row(row),m_col(col),m_pParent(parent),m_pexfatRoot(NULL){
			if(parent!=NULL){
				if(parent->m_pexfatRoot != NULL)
					m_pexfatRoot = parent->m_pexfatRoot;
			}
	}
};

class ExfatModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	ExfatModel(QObject *parent);
	~ExfatModel();
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

	void addRootDevice(QString,EXFATITEMTYPE);

	ExfatFSPrivate * findOutFSChild (QString abspath,EXFATITEMTYPE type)const;

	void refreshRootDevice();
	const static int ONCEBLOCK = 512;
	
	
	BOOL isDirectory(const char * dirpath);
	BOOL isRootItem(ExfatFSPrivate * priv)const;

	static QString covertHumanString(qlonglong orisize){
		double ksize = orisize / 1024.0;
		double msize = 0;
		if(orisize>4096LL){

			if(ksize > 1000){
				msize = ksize / 1024;
				double gsize = 0;
				if(msize > 1000){
					gsize = msize / 1024;
					return QString::number(gsize) + " GB";
				}
				return QString::number(msize) + " MB";
			}
			else{
				 return QString::number(ksize) + " KB";
			}
		}
		return QString::number(orisize) + " B";
	}

public slots:

	
private:
	QList<ExfatFSPrivate *> m_rootDrives;
	QList<ExfatFSPrivate *> m_allItems;
};

enum BGWORKMODE
{
	WMCALCSIZE=1,
	WMCOPYTOINNER,
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
	void setOutTargetDir(QString tardir){
		m_outtertarget = tardir;
	}
	void setInnerTargetDir(QString &targetdir){
		m_innertarget = targetdir;
	}
	
	DWORD getFileDirSize(const char * abspath);
	DWORD getOutSubItemCount(const char * abspath);
	int getInSubItemCount(QString rootdir,QString curdir,QList<QPair<QString,QString>> &subitems);
	int CopyFilesToInner(QList<QString> selOutPaths,QString intargetdir);
	int CopyFilesToOuter(QList<QPair<QString,QString>> & copyitems);
	void setWorkMode(BGWORKMODE mode){
		m_bgworkmode = mode;
	}
signals:
	void calcSizeRes(unsigned int);
	void calcItemCount(int,int);
	void updateSize(unsigned int);
	void copyDone();
protected:
	bool m_bIsQuit;
	QList<QString> m_selPaths;
	void run();

private:
	BGWORKMODE m_bgworkmode ;
	QString m_innertarget;
	QString m_outtertarget;
	int m_alreadyCopyedCount;
	int m_currentTotalCount;
	void CopyFileToInner(const char * outdir,const char * filename,const char * indir,const char * outrootdir);
	void CopyDirToInner(const char * outdir,const char * outdirname,const char * indir,const char * outrootdir);
};

#endif // EXFATFSMODEL_H
