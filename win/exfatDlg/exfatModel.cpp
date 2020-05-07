#ifdef UNICODE
#undef UNICODE
#endif

#include "exfatModel.h"
#include <qimage.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qdatetime.h>
#include <qdebug.h>
#include "exnotepad.h"
#include <qfileiconprovider.h>

#include <Windows.h>
extern "C"{
	#include "../../mkfs/mkexfat.h"
}

ExfatModel::ExfatModel(QObject *parent)
	: QAbstractItemModel(parent)
{
#if 0
	PFhahaLibGetProcStruct ProcAdd;
	if(LibHandle == NULL){
		LibHandle = LoadLibraryA("FhahaLib.dll");
		qDebug() << "user32 LibHandle = " << LibHandle;
		if(LibHandle){
			ProcAdd=(PFhahaLibGetProcStruct)GetProcAddress(LibHandle,"FhahaLibGetProcStruct");    
			qDebug() << "Get Func Address" <<  ProcAdd;
			if(ProcAdd!=NULL){
			
				pFunc = ProcAdd();
				qDebug() << "Get Pointer Address "<< pFunc;
			}
			else{
			
			}
		
		}
	}
#endif
	
}

ExfatModel::~ExfatModel()
{
	for(int i = 0; i < m_rootDrives.count(); i++)
	{
		ExfatFSPrivate * rootItem = m_rootDrives.at(i);
		if(rootItem->m_pexfatRoot){
			//TODO
			exfat_unmount(rootItem->m_pexfatRoot);
			free(rootItem->m_pexfatRoot);
		}
	}
	qDeleteAll(m_allItems);
}

ExfatFSPrivate * ExfatModel::findOutFSChild(QString abspath,EXFATITEMTYPE type) const{
	QList<ExfatFSPrivate *>::const_iterator pitem= m_allItems.begin();
	while(pitem != m_allItems.end()){
		if((*pitem)->match(abspath,type)){
			return *pitem;
		}
		pitem++;
	}
	return NULL;
}

BOOL ExfatModel::isDirectory(const char * dirpath)
{
#if 0
	if(pFunc){
		WIN32_FILE_ATTRIBUTE_DATA wfad;
		BOOL tRet = pFunc->GetFileAttributesEx(dirpath,GetFileExInfoStandard,&wfad);
		if(!tRet){
			return FALSE;
		}
		return wfad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

	}
#endif
	return FALSE;
}

QModelIndex ExfatModel::index(int row, int column,
                  const QModelIndex &parent) const
{
	//qDebug() << "call index with row" << row << " col " << column  << " parent row " << parent.row() << " column " << parent.column() ;

	if (!parent.isValid())
    {
        // 首层节点绑定关系
        if (row < m_rootDrives.size() && row != -1)
            return createIndex(row, column, m_rootDrives.at(row));
		else
			return QModelIndex();
    }
    else
    {
        // 其它层节点绑定关系
        if (parent.internalPointer() != nullptr)
        {
            ExfatFSPrivate * parentItemPtr = static_cast<ExfatFSPrivate *>(parent.internalPointer());
			
			QString childPath ;
			EXFATITEMTYPE childType = EXFTUNKNOWN;
			switch(parentItemPtr->fstype){
			case EXFTDRIVE:
				{
					//根据路径，row行数,返回绝对路径和类型
					char lpPath[MAX_PATH] = {0};
					struct exfat * ef = parentItemPtr->m_pexfatRoot;
					struct exfat_node * pparentnode = exfat_get_node(ef->root);
					struct exfat_node * node = NULL;
					int rc =  0;
					int searchidx = 0;
					if(pparentnode != NULL){
						struct exfat_iterator it;
						rc = exfat_opendir(ef, pparentnode, &it);
						if (rc != 0)
							break;
						while ((node = exfat_readdir(&it)))
						{
							if(row == searchidx){
								if(node->attrib & EXFAT_ATTRIB_DIR){
									childType = EXFTDIR;
									
								}
								else {
									childType = EXFTFILE;
								}
								int len = lstrlenW((LPCWSTR)&node->name);
								childPath = QString("/") +QString::fromUtf16((const  unsigned short *)&node->name,len);

								exfat_put_node(ef, node);
								break;
							}
							exfat_put_node(ef, node);
							searchidx++;
						}
						exfat_closedir(ef, &it);
						exfat_put_node(ef,pparentnode);
						if(childType != EXFTUNKNOWN){
							ExfatFSPrivate * pFind = NULL;
							pFind = findOutFSChild(childPath,childType);
							if(pFind == NULL){
								pFind = new ExfatFSPrivate(childPath,childType,row,column,parentItemPtr);
								(const_cast<ExfatModel*>(this))->m_allItems.append(pFind);
							}
							return createIndex(row,column,pFind);
						}

					}

					

				}
				break;
			case EXFTDIR:
				{
					//根据路径，row行数,返回绝对路径和类型
					char lpPath[MAX_PATH] = {0};
					int len = 0;
					struct exfat_node * pparentnode = NULL;
					struct exfat * ef = parentItemPtr->m_pexfatRoot;
					struct exfat_node * node = NULL;
					int rc = 0;
					int searchidx = 0;
					exfat_utf16_to_utf8(lpPath,(const le16_t *)parentItemPtr->absPath.data(),MAX_PATH,parentItemPtr->absPath.length());
					rc = exfat_lookup(ef,&pparentnode,lpPath);
					
					if(rc == 0){
						struct exfat_iterator it;
						rc = exfat_opendir(ef, pparentnode, &it);
						if (rc != 0)
							break;
						while ((node = exfat_readdir(&it)))
						{
							if(row == searchidx){
								int len = lstrlenW((LPCWSTR)&node->name);
								QString filename = QString::fromUtf16((const  unsigned short *)&node->name,len);
								if(node->attrib & EXFAT_ATTRIB_DIR){
									childType = EXFTDIR;
								}
								else 
									childType = EXFTFILE;

								childPath = parentItemPtr->absPath + "/" + filename ;//+ 

								exfat_put_node(ef, node);
								break;
							}
							exfat_put_node(ef, node);
							searchidx++;
						}
						exfat_closedir(ef, &it);
						exfat_put_node(ef,pparentnode);
						if(childType != EXFTUNKNOWN){
							ExfatFSPrivate * pFind = NULL;
							pFind = findOutFSChild(childPath,childType);
							if(pFind == NULL){
								pFind = new ExfatFSPrivate(childPath,childType,row,column,parentItemPtr);
								(const_cast<ExfatModel*>(this))->m_allItems.append(pFind);
							}
							return createIndex(row,column,pFind);
						}

					}

					

				}
				break;
			case EXFTFILE:
				{
					return QModelIndex();
				}
				break;
			default:
				return QModelIndex();
			}
        }
    }
    return QModelIndex();

}

char* GBKToUTF8( const char* chGBK ){	
	DWORD dWideBufSize=MultiByteToWideChar(CP_ACP, 0,(LPCSTR)chGBK,-1, NULL, 0);  	
	wchar_t * pWideBuf=new wchar_t[dWideBufSize];  	
	wmemset(pWideBuf, 0, dWideBufSize);  	
	MultiByteToWideChar(CP_ACP,0,(LPCSTR)chGBK,-1,pWideBuf,dWideBufSize); 	
	DWORD dUTF8BufSize=WideCharToMultiByte(CP_UTF8,0,(LPCWSTR)pWideBuf,-1,NULL,0,NULL,NULL);  	
	char * pUTF8Buf=new char[dUTF8BufSize];  	
	memset(pUTF8Buf, 0, dUTF8BufSize);  	
	DWORD chgSize = WideCharToMultiByte( CP_UTF8,0,(LPCWSTR)pWideBuf,-1,pUTF8Buf,dUTF8BufSize,NULL,NULL); 	
	//char tmpstr[256];
	//sprintf(tmpstr,"widechartomb ret %d\n",chgSize);
	//OutputDebugString(tmpstr);
	delete[]pWideBuf;	
	return pUTF8Buf;
}

void ExfatModel::editFile(const QString&)
{
	
}


QVariant ExfatModel::headerData(int section, Qt::Orientation orientation, int role ) const
{
	switch (role) {
    case Qt::DecorationRole:
        if (section == 0) {
            // ### TODO oh man this is ugly and doesn't even work all the way!
            // it is still 2 pixels off
            QImage pixmap(16, 1, QImage::Format_Mono);
            pixmap.fill(0);
            pixmap.setAlphaChannel(pixmap.createAlphaMask());
            return pixmap;
        }
        break;
    case Qt::TextAlignmentRole:
        return Qt::AlignLeft;
    }

    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QAbstractItemModel::headerData(section, orientation, role);
	QString returnValue;
    switch (section) {
    case 0: returnValue = tr("Name");
            break;
    case 1: returnValue = tr("Size");
            break;
    case 2: returnValue =
#ifdef Q_OS_MAC
               tr("Kind", "Match OS X Finder");
#else
               tr("Type", "All other platforms");
#endif
           break;
    // Windows   - Type
    // OS X      - Kind
    // Konqueror - File Type
    // Nautilus  - Type
    case 3: returnValue = tr("Date Modified");
            break;
    default: return QVariant();
    }
    return returnValue;
}
QVariant ExfatModel::data(const QModelIndex & index,
                                    int role ) const
{
	//qDebug() << "call data with row " << index.row() << " column " << index.column() << " role " << role ;
    if(!index.isValid())
    {
        return QVariant();
    }
    if( (role == Qt::TextAlignmentRole) || (role == Qt::ForegroundRole) )
    {
		if(index.column() != 0)
			return int(Qt::AlignHCenter | Qt::AlignVCenter);
		else
			return int(Qt::AlignLeft| Qt::AlignVCenter);
    }
	
	if(role == Qt::DecorationRole && index.column() == 0){
		ExfatFSPrivate * selPtr = static_cast<ExfatFSPrivate *>(index.internalPointer()); 
		EXFATITEMTYPE fstype = selPtr->fstype;
		QFileIconProvider icon_provider;
		switch(fstype){
		case EXFTFILE:
			{
				QIcon icon = icon_provider.icon(QFileIconProvider::File);
				return QVariant(icon);
			}
			break;
		case EXFTDRIVE:
			{
				QIcon icon = icon_provider.icon(QFileIconProvider::Drive);
				return QVariant(icon);
			}
			break;
		case EXFTDIR:
			{
				QIcon icon = icon_provider.icon(QFileIconProvider::Folder);
				return QVariant(icon);
			}
			break;
		}
	}
    if(role == Qt::DisplayRole)
    {
		//int rowcount = index.row();
		ExfatFSPrivate * selPtr = static_cast<ExfatFSPrivate *>(index.internalPointer()); 
		EXFATITEMTYPE fstype = selPtr->fstype;
		QString fspath = selPtr->absPath;
		//qDebug() << "want display " << fspath << " type" << fstype << " row " << index.row() << " column " << index.column() << " parent" << index.parent();
		switch(fstype){
		case EXFTDIR:
		case EXFTFILE:
			{
				//获取fspath文件的文件名、大小、类型、最后修改时间

				QString filename = selPtr->absPath;
				struct exfat_node * node;
				QString filesize = "0";
				QString filetype = tr("File");
				QString filelastmodifytime = "1980-01-01 00::00::00";
				char utf8str[MAX_PATH]={0};
				exfat_utf16_to_utf8(utf8str,(const le16_t *)fspath.data(),MAX_PATH,fspath.length());
				int rc = exfat_lookup(selPtr->m_pexfatRoot,&node,(const char *)&utf8str);
				do{
					if(rc ==0){
						char utf8str[MAX_PATH]={0};
						int len = 0;
						if(node->attrib & EXFAT_ATTRIB_DIR){
							filetype=tr("Dir");
							filesize = covertHumanString(node->size);
						}
						else{
							//struct stat tmpstat;
							//exfat_stat(selPtr->m_pexfatRoot,node,&tmpstat);
							filesize = covertHumanString(node->size);
							
						}
						filelastmodifytime = QDateTime::fromTime_t(node->mtime).toString("yyyy-MM-dd hh:mm:ss");
						len = lstrlenW((LPCWSTR)&node->name);
						filename = QString::fromUtf16((const  unsigned short *)&node->name,len);
						exfat_put_node(selPtr->m_pexfatRoot,node);
					}
				}while(0);
				switch(index.column()){
				case 0:
					return QVariant(filename);
					break;
				case 1:
					return QVariant(filesize);
					break;
				case 2:
					return QVariant(filetype);
					break;
				case 3:
					return QVariant(filelastmodifytime);
					break;
				default:
					qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!should not here!!!!!!!!!!!!!!!!!!!!!!!!!!";
					break;
				}
			}
			break;
		case EXFTDRIVE:
			{
				//QFileInfo info(fspath);
				QString filename = selPtr->m_pexfatRoot->label;
				QString filesize = "0";
				if(isRootItem(selPtr)){
					qlonglong tmpsize = (qlonglong)(selPtr->m_pexfatRoot->sb->cluster_count.__u32);
					filesize = covertHumanString( tmpsize << (selPtr->m_pexfatRoot->sb->spc_bits + selPtr->m_pexfatRoot->sb->sector_bits));
				}
				QString filetype = tr("Drive");
				QString filelastmodifytime = "";
				switch(index.column()){
				case 0:
					return QVariant(filename);
					break;
				case 1:
					return QVariant(filesize);
					break;
				case 2:
					return QVariant(filetype);
					break;
				case 3:
					return QVariant(filelastmodifytime);
					break;
				default:
					qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!here!!!!!!!!!!!!!!!!!!!!!!!!!!";
					break;
				}
			}
			break;
		}
			
        return QVariant("unknown");
    }
    //return QAbstractItemModel::data();
	return QVariant();
}

int ExfatModel::formatfs(const char * spec)
{
	struct exfat_dev * dev = exfat_open(spec, EXFAT_MODE_RW);
	if (dev == NULL)
		return 1;
	if (setupfs(dev, 9, 6, "NEWFS", 0x1314,0) != 0)
	{
		exfat_close(dev);
		return 1;
	}
	if (exfat_close(dev) != 0)
		return 1;
	
	addRootDevice(m_FSFileName,EXFTDRIVE);
	return 0;
}

int ExfatModel::resetfs()
{
	this->beginResetModel();
	for(int i = 0; i < m_rootDrives.count(); i++)
	{
		ExfatFSPrivate * rootItem = m_rootDrives.at(i);
		if(rootItem->m_pexfatRoot){
			//TODO
			exfat_unmount(rootItem->m_pexfatRoot);
			free(rootItem->m_pexfatRoot);
		}
	}
	m_rootDrives.clear();
	
	qDeleteAll(m_allItems);
	m_allItems.clear();

	formatfs(m_FSFileName.toStdString().c_str());
	this->addRootDevice(m_FSFileName.toStdString().c_str(),EXFTDRIVE);
	this->endResetModel();
	return 0;
}

QModelIndex ExfatModel::parent(const QModelIndex &child) const
{
	//qDebug() << "call parent with cild  row " << child.row() << " column " << child.column() ;
    //* 如果是无效的节点则返回无效节点
	if(!child.isValid())
	{
		//qDebug() << "no parent ";
		return QModelIndex();
	}

	ExfatFSPrivate* childData = static_cast<ExfatFSPrivate*>(child.internalPointer());
	for(int i = 0; i < m_rootDrives.count(); i++)
	{
		if(m_rootDrives[i] == childData) //* 如果是父节点（分组）则返回无效父节点（分组没有父节点）
		{
			//qDebug() << "no parent ";
			return QModelIndex();
		}
	}
	for(int i = 0; i<m_allItems.count();i++){
		ExfatFSPrivate* item = m_allItems[i];
		if(item == childData){
			if(item->m_pParent != NULL){
				//qDebug() << "find parent " << item->m_pParent;
				return createIndex(item->m_pParent->m_row,item->m_pParent->m_col,item->m_pParent);
			}
		}
		//return QModelIndex();
	}
	//* 未匹配到有效的父节点数据，返回无效节点
	//qDebug() << "no parent ";
	return QModelIndex();
}

void ExfatModel::refreshRootDevice()
{
	for(int i = 0; i < m_rootDrives.count(); i++)
	{
		ExfatFSPrivate * rootItem = m_rootDrives.at(i);
		if(rootItem->m_pexfatRoot){
			//TODO
			exfat_unmount(rootItem->m_pexfatRoot);
			free(rootItem->m_pexfatRoot);
		}
	}
	m_rootDrives.clear();
#if 0
	if(pFunc){
		int icount = 0;
		DWORD outDVs = pFunc->GetLogicalDrives();
		TCHAR tmpDv[3] = {'A',':',0};
		while(outDVs != 0l){
			if(outDVs & 1 != 0){
				tmpDv[0] = 'A' + icount;
				UINT DvType = pFunc->GetDriveType(tmpDv);
				switch(DvType){
				case DRIVE_FIXED:
				case DRIVE_NO_ROOT_DIR:
					{
						QString tmpDvName = (char *)tmpDv;
						addRootDevice(tmpDvName,EXFTDRIVE);
					}
					break;
				default:
					break;
				}
			}
			icount++;
			outDVs>>=1;
		}
	}
#endif
}

BOOL ExfatModel::isRootItem(ExfatFSPrivate * priv) const
{
	BOOL isrootitem = FALSE;
	QList<ExfatFSPrivate*>::const_iterator litem = m_rootDrives.begin();
	while(litem != m_rootDrives.end()){
		if((*litem)->match(priv->absPath, priv->fstype)){
			isrootitem = TRUE;
			break;
		}
		litem++;
	}
	return isrootitem;
}


void ExfatModel::addRootDevice(QString devname,EXFATITEMTYPE fstype)
{
	bool alreadyHas = false;
	QList<ExfatFSPrivate*>::iterator litem = m_rootDrives.begin();
	while(litem != m_rootDrives.end()){
		if((*litem)->match(devname, fstype)){
			alreadyHas = true;
			break;
		}
		litem++;
	}
	if(!alreadyHas){
		struct exfat * ef = (struct exfat * )malloc(sizeof(struct exfat));
		int mount_ret =  exfat_mount(ef, devname.toStdString().c_str(), "rw");
		if(mount_ret ==0){
			ExfatFSPrivate * newItem = new ExfatFSPrivate(devname,fstype,m_rootDrives.size(),0,NULL);
			newItem->m_pexfatRoot = ef;
			this->m_rootDrives.append(newItem);
			this->m_allItems.append(newItem);
		}
	}
}

int ExfatModel::rowCount(const QModelIndex &parent ) const
{
    if(!parent.isValid()){
		return m_rootDrives.size();
	}
	else{
		ExfatFSPrivate * parentData = static_cast<ExfatFSPrivate*>(parent.internalPointer());
		switch(parentData->fstype ){
		case EXFTDRIVE:
			{
				int listsize = 0;
				int rc;
				struct exfat * ef = parentData->m_pexfatRoot;
				struct exfat_node * prootdir  = exfat_get_node(ef->root);
				struct exfat_node* node;
				OutputDebugString("in drive rowCount\n");
				do{
					struct exfat_iterator it;
					rc = exfat_opendir(ef, prootdir, &it);
					if (rc != 0)
						break;
					while ((node = exfat_readdir(&it)))
					{
						listsize++;
						exfat_put_node(ef, node);
					}
					exfat_closedir(ef, &it);
				}while(0);
				exfat_put_node(ef,prootdir);
				return listsize;
			}
		case EXFTDIR:
			{
				//根据输入的目录绝对地址，获取下级文件和文件夹的数量
				int listsize = 0;
				int rc;
				struct exfat * ef = parentData->m_pexfatRoot;
				struct exfat_node * pdir ;
				char utf8str[MAX_PATH]={0};
				OutputDebugString("in dir rowCount\n");
				exfat_utf16_to_utf8(utf8str,(const le16_t *)parentData->absPath.data(),MAX_PATH,parentData->absPath.length());
				rc = exfat_lookup(ef,&pdir,utf8str);
				if(rc!=0)
					return 0;
				struct exfat_node* node;
				do{
					struct exfat_iterator it;
					rc = exfat_opendir(ef, pdir, &it);
					if (rc != 0)
						break;
					while ((node = exfat_readdir(&it)))
					{
						listsize++;
						exfat_put_node(ef, node);
					}
					exfat_closedir(ef, &it);
				}while(0);
				exfat_put_node(ef,pdir);
				
				return listsize;
				//return 0;
			}
			break;
		case EXFTFILE:
			return 0;
		default:
			return 0;
		} 
			
	}
	return 0;
}
int ExfatModel::columnCount(const QModelIndex &parent ) const
{
    return 4;
}

