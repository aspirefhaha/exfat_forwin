#ifndef EXFATFSMODEL_H
#define EXFATFSMODEL_H

#include <QAbstractItemModel>
#include <qthread.h>
#include <Windows.h>

enum EXFATITEMTYPE {
	OUTFTDRIVE=1,
	OUTFTSPEC,
	OUTFTDIR,
	OUTFTFILE,
	OUTFTUNKNOWN
};

struct ExfatFSPrivae 
{
	QString absPath;
	EXFATITEMTYPE fstype;
	int m_col;
	int m_row;
	ExfatFSPrivae * m_pParent;
	
	//QList<ExfatFSPrivae *> m_lsChildren;

	bool match(QString name,EXFATITEMTYPE fstype){
		return name==absPath && fstype==this->fstype;
	}

	explicit ExfatFSPrivae(QString devname,EXFATITEMTYPE tt,int row,int col,ExfatFSPrivae * parent)
		:absPath(devname),fstype(tt),m_row(row),m_col(col),m_pParent(parent){}
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

	ExfatFSPrivae * findOutFSChild (QString abspath,EXFATITEMTYPE type)const;

	void refreshRootDevice();
	const static int ONCEBLOCK = 512;
	
	
	BOOL isDirectory(const char * dirpath);

public slots:

	
private:
	QList<ExfatFSPrivae *> m_rootDrives;
	QList<ExfatFSPrivae *> m_allItems;
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
