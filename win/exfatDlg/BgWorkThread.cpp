#include "BgWorkThread.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <QtCore>
#include <QMessageBox>

void BgWorkThread::run()
{

	switch(m_bgworkmode){
	case WMCALCSIZE:
		{
			size_t totalsize = 0;
			QList<QString>::iterator iter = m_selPaths.begin();
			for(;iter!=m_selPaths.end() && !m_bIsQuit;iter++){
				totalsize += getFileDirSize((*iter).toStdString().c_str());
		
			}
			emit calcSizeRes(totalsize);
		}
		break;
	
	case WMCPTOEXF:
		{
			quint64 totalsize = 0;
			char tmpstr[100]={0};
			QList<QString>::Iterator iter = m_selPaths.begin();
			for(;iter!=m_selPaths.end() && !m_bIsQuit;iter++){
				totalsize += getOutSubItemSize((*iter));
			}
			qlonglong leftsize = ((qlonglong)exfat_count_free_clusters(ef)) << (ef->sb->spc_bits+ef->sb->sector_bits);
			//sprintf_s(tmpstr,100,"leftsize %lld\n",leftsize);
			//OutputDebugStringA(tmpstr);
			if (totalsize >= (leftsize - 1024 * 1024*1024LL) * 0.95)
			{
				emit updateSize(1000);
				emit copyDone(-1);
				break;
			}
			//emit calcItemCount(0,totalcount);
			emit updateSize(totalsize);
			
			
			CopyFilesToExfat(m_selPaths,m_selTargetDir);
			
			
			emit copyDone(0);
			//m_currentTotalCount = totalcount;
			//CopyFilesToExfat(m_selPaths,m_innertarget);
		}
		break;
	case WMCOPYTOOUTER:
		{
			qlonglong totalsize = 0;
			QList<QPair<QString,QString>> selSources;
			struct exfat_node * pnode;
			for(int i = 0 ;i<m_selPaths.size();i++){
				if(exfat_lookup(ef,&pnode,m_selPaths.at(i).toStdString().c_str())==0){
					totalsize += pnode->size;
					selSources.append(QPair<QString,QString>(m_selPaths.at(i),m_outtertarget));
					exfat_put_node(ef,pnode);
				}
			}
			
			emit updateSize(totalsize);
			CopyFilesToOuter(selSources);
		}
		break;
	}
}

quint64 BgWorkThread::CopyDirToExfat(QString &sourcedir,QString & targetdir,quint64 cursize)
{
	QFileInfo fileinfo(sourcedir);
	int sendIdx = 0;
	char utf8str[EXFAT_UTF8_NAME_BUFFER_MAX]={0};
	
	QString targetdirname ;
	if(targetdir.endsWith("/"))
		targetdirname = targetdir + fileinfo.fileName();
	else
		targetdirname = targetdir + "/" + fileinfo.fileName();
	exfat_utf16_to_utf8(utf8str,(const le16_t *)targetdirname.data(),EXFAT_UTF8_NAME_BUFFER_MAX,targetdirname.length());
	if(exfat_mkdir(ef,utf8str)!=0){
		QString msgErr = QString(tr("Create Dir %1 Failed")).arg(targetdirname);
		
		QString title = tr("Mkdir Err");
		emit warningMsg(title,msgErr);
		return 0;
	}
	QDir dir(sourcedir);
	quint64 localcursize = cursize;
    dir.setFilter(QDir::Dirs|QDir::Files);
    foreach(QFileInfo fullDir, dir.entryInfoList())
    {
        if(fullDir.fileName() == "." || fullDir.fileName() == "..") continue;
		QString subtargetdir = targetdirname ;
		QString curdirname = fullDir.absoluteFilePath();
		QFileInfo subfileinfo(curdirname);
		if(subfileinfo.isDir()){
			localcursize =  CopyDirToExfat(curdirname,subtargetdir,localcursize);
		}
		else{
			localcursize +=  CopyFileToExfat(curdirname,subtargetdir,localcursize);
			if(sendIdx++%10==0){
				emit updateProg(localcursize);
				msleep(1);
			}
		}
    }
	emit updateProg(localcursize);
	return localcursize;
}

quint64  BgWorkThread::CopyFileToExfat(QString &sourcefile,QString & targetdir,quint64 cursize)
{
	QFile sourcef(sourcefile);
	QFileInfo fileinfo(sourcefile);
	int sendidx = 0;
	
	bool isOk =sourcef.open(QIODevice::ReadOnly );
    char * pbuf;
	off_t curpos = 0;
	int bufsize = 2 << (ef->sb->spc_bits + ef->sb->sector_bits -1);
	char utf8str[EXFAT_UTF8_NAME_BUFFER_MAX]={0};
	QString targetfilename ;
	if(targetdir == "/" ){
		targetfilename = "/" + fileinfo.fileName();
	}
	else{
		targetfilename = targetdir + "/" + fileinfo.fileName();
	}
	exfat_utf16_to_utf8(utf8str,(const le16_t *)targetfilename.data(),EXFAT_UTF8_NAME_BUFFER_MAX,targetfilename.length());
    int mknoderet = exfat_mknod(ef,utf8str);
	if(mknoderet==0){
		struct exfat_node * node;
		
		if(exfat_lookup(ef,&node,utf8str)==0){
			pbuf = (char *)malloc(bufsize);
			do{
				qint64 onceread = 0;
				onceread = sourcef.read(pbuf,bufsize);
				if(onceread<=0){
					break;
				}
				size_t writensize = exfat_generic_pwrite(ef,node,pbuf,onceread,curpos);
				if(writensize == onceread){
					curpos += writensize;
					cursize += writensize;
					if(sendidx++%10==0){
						emit updateProg(cursize);
						msleep(1);
					}
				}
				else{
					break;
				}
			}while(1);
			free(pbuf);
			exfat_flush_node(ef,node);
			exfat_put_node(ef,node);
		}
	}
	else{

	}
	return curpos;
	
}

int BgWorkThread::CopyFilesToOuter(QList<QPair<QString,QString>> & copyitems)
{
	m_alreadyCopyedCount = 0;
	off_t curpos;
	char tmpbuf[64*1024];
	m_currentTotalCount = copyitems.size();
	QPair<QString,QString> copyitem;
	qlonglong  totalpossize = 0 ;
	int sendeventcount = 0;
	foreach(copyitem,copyitems){
		QString sourcename = copyitem.first;
		exfat_node * pnode;
		QFileInfo appInfo(sourcename);
		QFile targetfile(copyitem.second + appInfo.baseName() + "." + appInfo.suffix());
		if(!targetfile.open(QIODevice::Truncate | QIODevice::WriteOnly))
			continue;

		if(exfat_lookup(ef,&pnode,sourcename.toStdString().c_str())==0){
			curpos = 0;
			size_t readsize = exfat_generic_pread(ef,pnode,tmpbuf,64*1024,curpos);
			while(readsize>0){
				targetfile.write(tmpbuf,readsize);
				curpos += readsize;
				totalpossize += readsize;
				readsize = exfat_generic_pread(ef,pnode,tmpbuf,64*1024,curpos);
				if(sendeventcount++%100==0){
					emit updateProg(totalpossize);
					msleep(100);
				}
			}
			exfat_flush_node(ef,pnode);
			exfat_put_node(ef,pnode);
			targetfile.flush();
			targetfile.close();
		}

	}
	emit copyDone(0);
	return 0;
}

int BgWorkThread::CopyFilesToExfat(QList<QString> selOutPaths,QString intargetdir)
{
	m_alreadyCopyedCount = 0;
	QList<QString>::Iterator iter = m_selPaths.begin();
	quint64 cursize = 0;
	int sendidx = 0;
	for(;iter!=m_selPaths.end() && !m_bIsQuit;iter++){
		QFileInfo fileinfo(*iter);
		if(fileinfo.isDir()){ // is a dir
			CopyDirToExfat(*iter,intargetdir,cursize);
		}
		else{ // is a file 
			cursize += CopyFileToExfat(*iter,intargetdir,cursize);
			if(sendidx++%10==0){
				emit updateProg(cursize);
				msleep(1);
			}
		}
	}
	emit copyDone(0);
	return 0;
}

BgWorkThread::BgWorkThread(QObject * parent):QThread(parent),m_bIsQuit(false),m_bgworkmode(WMCALCSIZE)
{

}

BgWorkThread::~BgWorkThread()
{

}

int BgWorkThread::getInSubItemCount(QString rootdir,QString curdir,QList<QPair<QString,QString>> &subitems)
{
	QDir dir(curdir);
	subitems.append(QPair<QString,QString>(curdir,rootdir));
    dir.setFilter(QDir::Dirs|QDir::Files);
    foreach(QFileInfo fullDir, dir.entryInfoList())
    {
        if(fullDir.fileName() == "." || fullDir.fileName() == "..") continue;
        subitems.append(QPair<QString,QString>(fullDir.absoluteFilePath(),rootdir));
        getInSubItemCount(rootdir,fullDir.absoluteFilePath(), subitems);
    }

    return subitems.count();
}

qlonglong BgWorkThread::getOutSubItemSize(QString &abspath)
{
	QFileInfo  orifileinfo(abspath);
	quint64 size = 0;
	if(orifileinfo.isDir()){
		QDir dir(abspath);
		//dir.entryInfoList(QDir::Files)返回文件信息
		foreach(QFileInfo fileInfo, dir.entryInfoList(QDir::Files))
		{
			//计算文件大小
			size += fileInfo.size();
		}
		//dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot)返回所有子目录，并进行过滤
		foreach(QString subDir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
		{
			//若存在子目录，则递归调用dirFileSize()函数
			QString subpath = abspath + QDir::separator() + subDir;
			size += getOutSubItemSize(subpath);
		}
	}
	else{
		size = orifileinfo.size();
	}
	return size;

}


size_t BgWorkThread::getFileDirSize(const char * abspath)
{
	size_t totalsize = 0;
	if(strlen(abspath)>=EXFAT_UTF8_NAME_BUFFER_MAX)
		return 0;

#if 0

	WIN32_FILE_ATTRIBUTE_DATA wfad;
	BOOL tRet = ExfatModel::pFunc->GetFileAttributesEx(abspath,GetFileExInfoStandard,&wfad);
	if(!tRet){
		return 0;
	}
	if(wfad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
		char tmppath[EXFAT_UTF8_NAME_BUFFER_MAX];
		strcpy_s(tmppath,EXFAT_UTF8_NAME_BUFFER_MAX,abspath);
		int len = strlen(tmppath);
		if(tmppath[len-1]!='/')
		{
			tmppath[len]='/';
			tmppath[len+1]='*';
			tmppath[len+2] = 0;
		}
		else{
			tmppath[len]='*';
			tmppath[len+1]=0;
		}
		WIN32_FIND_DATA fd;
		long long hFindFile = ExfatModel::pFunc->FindFirstFile(tmppath,&fd);
		if(hFindFile == -1 || hFindFile == 0){
			ExfatModel::pFunc->CloseHandle(hFindFile);
			return totalsize;
		}
		else{
			bool isFinished = false;
			BOOL bIsDirectory = FALSE;
			int idx = 0;
			while(!isFinished){  
				bIsDirectory = ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);  
              
				//如果是.或..  
				if( bIsDirectory  
					&& (strcmp(fd.cFileName, ".")==0 || strcmp(fd.cFileName, "..")==0))   
				{         
					isFinished = (ExfatModel::pFunc->FindNextFile(hFindFile, &fd) == FALSE);  
					continue;  
				} 
				char subAbsPath[EXFAT_UTF8_NAME_BUFFER_MAX];
				sprintf(subAbsPath,"%s/%s",abspath,fd.cFileName);
				totalsize += getFileDirSize(subAbsPath);
				emit updateSize(totalsize);
				msleep(1);
				isFinished = (ExfatModel::pFunc->FindNextFile(hFindFile, &fd) == FALSE);  
			}  
          
			ExfatModel::pFunc->FindClose(hFindFile);  
		}
	}
	else {
		long long thisSize = wfad.nFileSizeHigh ;
		thisSize <<= 32;
		thisSize += wfad.nFileSizeLow;
		thisSize /= ExfatModel::ONCEBLOCK;
		totalsize = thisSize ;
	}
	
#endif
	return totalsize;
}

