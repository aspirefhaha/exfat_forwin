#ifndef EXFATFSMODEL_H
#define EXFATFSMODEL_H
#define __bool_true_false_are_defined 1
extern "C"{
#include "exfat.h"
}
#include <QAbstractItemModel>
#include <qthread.h>
#include <Windows.h>
#define XDISKDEFAULTKEY	"123456"
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

	int resetfs();

	int formatfs(const char * spec);

	void addRootDevice(QString,EXFATITEMTYPE);

	ExfatFSPrivate * findOutFSChild (QString abspath,EXFATITEMTYPE type)const;

	void refreshRootDevice();
	void editFile(const QString&);
	const static int ONCEBLOCK = 512;
	
	
	BOOL isDirectory(const char * dirpath);
	BOOL isRootItem(ExfatFSPrivate * priv)const;

	void notifyChange(const QModelIndex & someindex,const QModelIndex &rbindex){
		emit dataChanged(someindex,rbindex);
	}

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

#endif // EXFATFSMODEL_H
