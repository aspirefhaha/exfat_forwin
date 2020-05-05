#include "BgWorkThread.h"
#include <windows.h>
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

			QList<QString>::Iterator iter = m_selPaths.begin();
			for(;iter!=m_selPaths.end() && !m_bIsQuit;iter++){
				totalsize += getOutSubItemSize((*iter));
			}
			
			//emit calcItemCount(0,totalcount);
			emit updateSize(totalsize);
			
			
			CopyFilesToExfat(m_selPaths,m_selTargetDir);
			
			
			emit copyDone();
			//m_currentTotalCount = totalcount;
			//CopyFilesToExfat(m_selPaths,m_innertarget);
		}
		break;
	case WMCOPYTOOUTER:
		{
			int totalcount = 0;
			QList<QString>::Iterator iter = m_selPaths.begin();
			QList<QPair<QString,QString>> selSources;
			for(;iter!=m_selPaths.end() && !m_bIsQuit;iter++){
				QString curPath = (*iter);
				QDir updir(curPath);
				if(updir.cdUp()){
					QString filename;
					totalcount += getInSubItemCount(updir.absoluteFilePath(filename),curPath,selSources);
				}
				
			}
			emit calcItemCount(0,selSources.size());
			
			CopyFilesToOuter(selSources);
		}
		break;
	}
}

quint64 BgWorkThread::CopyDirToExfat(QString &sourcedir,QString & targetdir,quint64 cursize)
{
	QFileInfo fileinfo(sourcedir);
	char utf8str[MAX_PATH]={0};
	
	QString targetdirname ;
	if(targetdir.endsWith("/"))
		targetdirname = targetdir + fileinfo.fileName();
	else
		targetdirname = targetdir + "/" + fileinfo.fileName();
	exfat_utf16_to_utf8(utf8str,(const le16_t *)targetdirname.data(),MAX_PATH,targetdirname.length());
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
			localcursize +=  CopyDirToExfat(curdirname,subtargetdir,localcursize);
		}
		else{
			localcursize +=  CopyFileToExfat(curdirname,subtargetdir,localcursize);
			emit updateSize(localcursize);
		}
    }
	emit updateSize(localcursize);
	return localcursize;
}

quint64  BgWorkThread::CopyFileToExfat(QString &sourcefile,QString & targetdir,quint64 cursize)
{
	QFile sourcef(sourcefile);
	QFileInfo fileinfo(sourcefile);
	
	bool isOk =sourcef.open(QIODevice::ReadOnly | QIODevice::Text);
    char * pbuf;
	off_t curpos = 0;
	int bufsize = 2 << (ef->sb->spc_bits + ef->sb->sector_bits -1);
	char utf8str[MAX_PATH]={0};
	QString targetfilename ;
	if(targetdir == "/" ){
		targetfilename = "/" + fileinfo.fileName();
	}
	else{
		targetfilename = targetdir + "/" + fileinfo.fileName();
	}
	exfat_utf16_to_utf8(utf8str,(const le16_t *)targetfilename.data(),MAX_PATH,targetfilename.length());
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
					emit updateProg(cursize);
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
	m_currentTotalCount = copyitems.size();
	QPair<QString,QString> copyitem;
	foreach(copyitem,copyitems){
		QFileInfo fileinfo(copyitem.first);
		char outpath[MAX_PATH]={0};
		char tmpdirname[MAX_PATH]={0};
		strcpy(tmpdirname,copyitem.second.toLocal8Bit().data());
		int rootlen = strlen(tmpdirname);
		strcpy(tmpdirname,copyitem.first.toLocal8Bit().data());
		char * pdirname = tmpdirname + rootlen;
		sprintf_s(outpath,MAX_PATH,"%s/%s",m_outtertarget.toStdString().c_str(),pdirname);
#if 0
		if(fileinfo.isDir()){
			BOOL tExist = ExfatModel::pFunc->PathFileExists(outpath);
			if(!tExist){
				ExfatModel::pFunc->CreateDirectory(outpath,NULL);
			}
			m_alreadyCopyedCount++;
			emit calcItemCount(m_alreadyCopyedCount,m_currentTotalCount);
		}
		else{
			do{
				QFile ifile(copyitem.first);
				if(!ifile.open(QIODevice::ReadOnly)){
					break;
				}
				HANDLE th = (HANDLE)ExfatModel::pFunc->CreateFile(outpath,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_FLAG_RANDOM_ACCESS,NULL);
				if(th == (HANDLE)-1 || th == (HANDLE)0){
					break;
				}
				char tmpdata[ExfatModel::ONCEBLOCK];
				int readlen = 0;
				DWORD tRetLen = 0;
				do{
					readlen = (int)ifile.read(tmpdata,ExfatModel::ONCEBLOCK);
					if(readlen >0){
						ExfatModel::pFunc->WriteFile((long long )th,tmpdata,readlen,&tRetLen,NULL);
					}
				}while(readlen > 0 && tRetLen == readlen);
				ExfatModel::pFunc->CloseHandle((long long)th);
				ifile.close();

			}while(0);
			m_alreadyCopyedCount++;
			emit calcItemCount(m_alreadyCopyedCount,m_currentTotalCount);
		}
#endif
	}
	emit copyDone();
	return 0;
}

int BgWorkThread::CopyFilesToExfat(QList<QString> selOutPaths,QString intargetdir)
{
	m_alreadyCopyedCount = 0;
	QList<QString>::Iterator iter = m_selPaths.begin();
	quint64 cursize = 0;
	for(;iter!=m_selPaths.end() && !m_bIsQuit;iter++){
		QFileInfo fileinfo(*iter);
		if(fileinfo.isDir()){ // is a dir
			CopyDirToExfat(*iter,intargetdir,cursize);
		}
		else{ // is a file 
			cursize += CopyFileToExfat(*iter,intargetdir,cursize);
			emit updateProg(cursize);
		}
	}
	emit copyDone();
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
	WIN32_FILE_ATTRIBUTE_DATA wfad;
	if(strlen(abspath)>=MAX_PATH)
		return 0;
#if 0
	BOOL tRet = ExfatModel::pFunc->GetFileAttributesEx(abspath,GetFileExInfoStandard,&wfad);
	if(!tRet){
		return 0;
	}
	if(wfad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
		char tmppath[MAX_PATH];
		strcpy_s(tmppath,MAX_PATH,abspath);
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
				char subAbsPath[MAX_PATH];
				sprintf(subAbsPath,"%s/%s",abspath,fd.cFileName);
				totalsize += getFileDirSize(subAbsPath);
				emit updateSize(totalsize);
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

